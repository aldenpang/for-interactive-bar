#ifndef _DATA_CENTER_H_
#define _DATA_CENTER_H_


class DataCenter
{
public:
	~DataCenter(){}

	static DataCenter& GetSingleton()
	{
		static DataCenter singleton;
		return singleton;
	}

	int mThreshold1;
	int mThreshold2;
	int mCannyApertureSize;
	int mPixelStep;

	int mVideoWidth;
	int mVideoHeight;

	void LoadConfig();
	void SaveConfig();

	void LoadVideoSize();
private:
	DataCenter()
	: mThreshold1(50)
	, mThreshold2(150)
	, mCannyApertureSize(3)
	, mPixelStep(5)
	, mVideoWidth(320)
	, mVideoHeight(240)
	, mFileName("config.cfg")
	{
	
	}

	std::string mFileName;
};

void DataCenter::LoadConfig()
{
	FILE* fp = fopen(mFileName.c_str(), "rb");
	if ( fp )
	{
		int value;
		fread(&value, sizeof(value), 1, fp);
		mVideoWidth = value;
		fread(&value, sizeof(value), 1, fp);
		mVideoHeight = value;
		fread(&value, sizeof(value), 1, fp);
		mThreshold1 = value;
		fread(&value, sizeof(value), 1, fp);
		mThreshold2 = value;
		fread(&value, sizeof(value), 1, fp);
		mPixelStep = value;
		fclose(fp);
	}
}

void DataCenter::SaveConfig()
{
	FILE* fp = fopen(mFileName.c_str(), "wb");
	if ( fp )
	{
		fwrite(&mVideoWidth, sizeof(mVideoWidth), 1, fp);
		fwrite(&mVideoHeight, sizeof(mVideoHeight), 1, fp);
		fwrite(&mThreshold1, sizeof(mThreshold1), 1, fp);
		fwrite(&mThreshold2, sizeof(mThreshold2), 1, fp);
		fwrite(&mPixelStep, sizeof(mPixelStep), 1, fp);
		fclose(fp);
	}
}

void DataCenter::LoadVideoSize()
{
	FILE* fp = fopen("videoSize.cfg", "rb");
	if ( fp )
	{
		char width[32]={0};
		char height[32]={0};
		fscanf(fp, "width=%s\nheight=%s", width, height);
		mVideoWidth = atoi(width);
		mVideoHeight = atoi(height);
	}
}
#define DATACENTER DataCenter::GetSingleton()

#endif