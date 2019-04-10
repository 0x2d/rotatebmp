#include<iostream>
#include<fstream>
using namespace std;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

char *srcFile;
char *srcFile2;

DWORD swap_32(DWORD a)
{
	return ((a & 0xff000000) >> 24) | \
		((a & 0x00ff0000) >> 8) | \
		((a & 0x0000ff00) << 8) | \
		((a & 0x000000ff) << 24);
}

WORD swap_16(WORD a)
{
	return ((a & 0x00ff) << 8) | \
		((a & 0xff00) >> 8);
}

//λͼ�ļ�ͷ����
typedef struct tagBITMAPFILEHEADER {
	//  WORD bfType;//������ȡ���ṹ���оͲ�������
	DWORD bfSize;//�ļ���С
	WORD bfReserved1;//������
	WORD bfReserved2;//������
	DWORD bfOffBits;//���ļ�ͷ��ʵ��λͼ���ݵ�ƫ���ֽ���
} BITMAPFILEHEADER;

//λͼ��Ϣͷ����
typedef struct tagBITMAPINFOHEADER {
	DWORD biSize;//��Ϣͷ��С
	DWORD biWidth;//ͼ����
	DWORD biHeight;//ͼ��߶�
	WORD biPlanes;//λƽ����
	WORD biBitCount;//ÿ����λ��
	DWORD biCompression;//ѹ������
	DWORD biSizeImage;//ѹ��ͼ���С�ֽ���
	DWORD biXPelsPerMeter;//ˮƽ�ֱ���
	DWORD biYPelsPerMeter;//��ֱ�ֱ���
	DWORD biClrUsed;//λͼʵ���õ���ɫ����
	DWORD biClrImportant;//��λͼ����Ҫ��ɫ����
} BITMAPINFOHEADER;

//������Ϣ
typedef struct tagIMAGEDATA
{
	BYTE blue;
	BYTE green;
	BYTE red;
} DATA;

typedef struct tagIMAGEDATA_32
{
	BYTE blue;
	BYTE green;
	BYTE red;
	BYTE alpha;
} DATA_32;

class BmpFile
{
public:
	void rotate90()
	{
		ofstream fout(srcFile2, ios::out | ios::binary);
		if(strInfo.biBitCount == 32)
		{
			DATA_32* target = new DATA_32[pSize];
			memset(target, 0, sizeof(DATA_32)*pSize);
			for (int i = 0; i < h; i++)
			{
				for (int j = 0; j < w; j++)
					target[j*h + i] = src_32[i*w + w - j - 1];
			}
			BITMAPFILEHEADER newHead = strHead;
			BITMAPINFOHEADER newInfo = strInfo;
			newHead.bfSize = (DWORD)(newHead.bfSize);
			newInfo.biHeight = (DWORD)w;
			newInfo.biWidth = (DWORD)h;
			newInfo.biSizeImage = (DWORD)(pSize * 4);
			fout.write((char *)&bfType, sizeof(WORD));
			fout.write((char *)&newHead, sizeof(tagBITMAPFILEHEADER));
			fout.write((char *)&newInfo, sizeof(tagBITMAPINFOHEADER));
			fout.write(bytes, 84);
			fout.write((char *)target, sizeof(DATA_32)*pSize);
			delete[]target;
		}
		else
		{
			DATA* target = new DATA[pSize];
			memset(target, 0, sizeof(DATA)*pSize);
			for (int i = 0; i < h; i++)
			{
				for (int j = 0; j < w; j++)
					target[j*h + i] = src[i*w + w - j - 1];
			}
			BITMAPFILEHEADER newHead = strHead;
			BITMAPINFOHEADER newInfo = strInfo;
			newHead.bfSize = (DWORD)(newHead.bfSize);
			newInfo.biHeight = (DWORD)w;
			newInfo.biWidth = (DWORD)h;
			newInfo.biSizeImage = (DWORD)(pSize * 3);
			fout.write((char *)&bfType, sizeof(WORD));
			fout.write((char *)&newHead, sizeof(tagBITMAPFILEHEADER));
			fout.write((char *)&newInfo, sizeof(tagBITMAPINFOHEADER));
			fout.write((char *)target, sizeof(DATA)*pSize);
			delete[]target;
		}
		fout.close();
	}
	void readfile()
	{
		ifstream fin(srcFile, ios::in | ios::binary);
		if (fin)
		{
			fin.read((char *)&bfType, sizeof(WORD));
			fin.read((char *)&strHead, sizeof(BITMAPFILEHEADER));
			fin.read((char *)&strInfo, sizeof(BITMAPINFOHEADER));
			h = strInfo.biHeight;
			w = strInfo.biWidth;
			if (w % 4 == 0)
				mW = w;
			else
				mW = (w / 4 + 1) * 4;//ʾ��ͼƬ��û��4Byte����
			pSize = strInfo.biSizeImage / (strInfo.biBitCount / 8);
			if (strInfo.biBitCount == 32)
			{
				bytes = new char[100];
				fin.read(bytes, 84);
				src_32 = new DATA_32[pSize];
				fin.read((char *)src_32, sizeof(DATA_32)*pSize);				
			}
			else
			{
				src = new DATA[pSize];
				fin.read((char *)src, sizeof(DATA)*pSize);
			}
			fin.close();
		}
	}
private:
	BITMAPFILEHEADER strHead;//12
	BITMAPINFOHEADER strInfo;//40
	int h, w, pSize;
	int mW, mSize;
	char *bytes;
	WORD bfType;//2
	DATA* src;
	DATA_32* src_32;
};

int main(int argc, char* argv[])
{
	srcFile = argv[1];
	srcFile2 = argv[2];
	BmpFile BF;
	BF.readfile();
	BF.rotate90();
	return 0;
}