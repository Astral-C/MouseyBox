#ifdef ENABLE_XM
#include <math.h>
#include <algorithm>
#include <system/Log.hpp>
#include <audio/TrackerXM.hpp>
#include <xm.h>

namespace mb::Audio {

    void TrackerXM::Load(uint8_t* data, std::size_t size){
        mPrescanData = new uint8_t[XM_PRESCAN_DATA_SIZE];

        if(!xm_prescan_module((char*)data, size, (xm_prescan_data_t*)mPrescanData)){
          mb::Log::Error("Failed to Load XM Module from data");
          return;
        }

        mCtxData = new uint8_t[xm_size_for_context((xm_prescan_data_t*)mPrescanData)];
        mXMHandle = xm_create_context((char*)mCtxData, (xm_prescan_data_t*)mPrescanData, (const char*)data, size, 44100);

        mLastPattern = xm_get_module_length(mXMHandle)-1;
        mLastPatternRows = xm_get_number_of_rows(mXMHandle, mLastPattern);
    }

    void TrackerXM::Reset(){
        //xm_seek(mXMHandle, 0, 0, 0);
    }

    void TrackerXM::Load(std::filesystem::path path){
        bStream::CFileStream stream(path.string(), bStream::OpenMode::In);

        std::size_t streamSize = stream.getSize();

        uint8_t* data = new uint8_t[streamSize]{0};
        stream.readBytesTo(data, streamSize);

        mPrescanData = new uint8_t[XM_PRESCAN_DATA_SIZE];

        if(!xm_prescan_module((char*)data, streamSize, (xm_prescan_data_t*)mPrescanData)){
          mb::Log::Error("Failed to Load XM Module from data");
          return;
        }

        mCtxData = new uint8_t[xm_size_for_context((xm_prescan_data_t*)mPrescanData)];
        mXMHandle = xm_create_context((char*)mCtxData, (xm_prescan_data_t*)mPrescanData, (const char*)data, streamSize, 44100);

        mLastPattern = xm_get_module_length(mXMHandle)-1;
        mLastPatternRows = xm_get_number_of_rows(mXMHandle, mLastPattern);

        delete[] data;
    }

    void TrackerXM::Mix(uint8_t* frameBuffer, uint8_t* data, int len){
        int16_t* sampleBuffer = reinterpret_cast<int16_t*>(data);

        float* samples = reinterpret_cast<float*>(frameBuffer);
        memset(samples, 0, len / sizeof(float));

        xm_generate_samples(mXMHandle, samples, len/sizeof(float));

        for(int sample = 0; sample < (len/2); sample+=2){
            sampleBuffer[sample] += (int16_t)((samples[sample] * INT16_MAX) * mVolume);
            sampleBuffer[sample+1] += (int16_t)((samples[sample+1] * INT16_MAX) * mVolume);
        }
    }

    void TrackerXM::Loop(){
        // This is automatic
        //xm_seek(mXMHandle, 0, 0, 0);
    }

    bool TrackerXM::AtEnd() {
       uint8_t curPattern, curRow;
       xm_get_position(mXMHandle, nullptr, &curPattern, &curRow, nullptr);
       return mLastPattern == curPattern && mLastPatternRows == curRow;
    }

    TrackerXM::TrackerXM(){}

    TrackerXM::~TrackerXM(){
        free(mXMHandle);
        delete[] mPrescanData;
        delete[] mCtxData;
    }
}
#endif
