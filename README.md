# libuds
UDS类库
```
#include <libcan/libcan.h>
#include <libuds/libuds.h>
#ifdef _DEBUG
#pragma comment(lib, "libcand.lib")
#pragma comment(lib, "libudsd.lib")
#else
#pragma comment(lib, "libcan.lib")
#pragma comment(lib, "libuds.lib")
#endif

int main()
{
    auto candev = can::autoReleaseNew(can::ZLG_USBCANFD200U);
    uds::CommunicationId cid;
    cid.request.mode = uds::RequestMode::PHYSICAL;
    cid.request.physical = 0x727;
    cid.response = 0x7a7;
    auto getKey = [](int level, const unsigned char* seedData, int seedSize, unsigned char* keyData, int* keySize){
        unsigned int mask = 0x10000000;
		unsigned int seed = 0;
		unsigned int key = 0;

		for (int i = 0; i < 4; ++i) {
			seed |= static_cast<unsigned int>(seedData[i] << 8 * (3 - i));
		}

		for (int i = 0; i < 35; ++i) {
			if (seed & 0xf0000000) {
				seed = seed << 1;
				seed = seed ^ mask;
			}
			else {
				seed = seed << 1;
			}
		}
		key = seed;

		if (keySize) {
			*keySize = 4;
		}

		for (int i = 0; i < 4; ++i) {
			keyData[i] = key >> (8 * (3 - i));
		}

		return true;
    };

    auto udsdev = uds::autoReleaseNew(candev, 0, can::CANFD, cid, getKey);
    // 过滤报文ID
    candev->setFilterId({ 0x727, 0x7a7 });
    // 控制台输出日志
    candev->setOutputLog(true);
    // 注册报文回调
    candev->setMsgProc([](const char* direction, const can::Msg& msg) {
	    std::cout << direction << "->" << msg << std::endl;
    });

    can::Device dev;
    if (!candev->open(dev)) {
	    std::cout << "打开CAN卡失败," << candev->getLastError() << std::endl;
	    return -1;
    }

    Sleep(3000);
    do 
    {
        // 下面演示如何将bin文件下载到ECU中
	    if (!udsdev->diagnosticSessionControl(0x03)) {
		    std::cout << "诊断会话控制0x03失败," << udsdev->getLastError() << std::endl;
		    break;
	    }

	    if (!udsdev->diagnosticSessionControl(0x02)) {
		    std::cout << "诊断会话控制0x02失败," << udsdev->getLastError() << std::endl;
		    break;
	    }
	    Sleep(1000);

	    if (!udsdev->securityAccess(0x11)) {
		    std::cout << "安全访问失败," << udsdev->getLastError() << std::endl;
		    break;
	    }

	    uint8_t data[] = { 0x25, 0x07, 0x31, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f };
	    if (!udsdev->writeDataByIdentifier(0xf1, 0x84, data, sizeof(data))) {
		    std::cout << "写数据标识符失败," << udsdev->getLastError() << std::endl;
		    break;
	    }

	    if (!udsdev->requestDownload(0x00, 0x44, 0x00, 0x0c04)) {
		    std::cout << "请求下载1失败," << udsdev->getLastError() << std::endl;
		    break;
	    }

	    FILE* fp0 = fopen("1-AVM_A512E01905_V001.bin", "rb");
	    if (!fp0) {
		    std::cout << "打开1-AVM_A512E01905_V001.bin失败\n";
		    break;
	    }

	    fseek(fp0, SEEK_SET, SEEK_END);
	    auto size0 = ftell(fp0);
	    fseek(fp0, SEEK_SET, SEEK_SET);

	    auto buffer0 = std::make_unique<uint8_t[]>(size0);
	    fread(buffer0.get(), 1, size0, fp0);
	    fclose(fp0);

	    uint8_t block0 = 0;
	    auto success0 = true;
	    for (size_t i = 0; i < size0; i += 256) {
		    auto written = size0 - i > 256 ? 256 : size0 - i;
		    if (!udsdev->transferData(++block0, &buffer0[i], written)) {
			    success0 = false;
			    std::cout << "传输数据0失败," << udsdev->getLastError() << std::endl;
			    break;
		    }
	    }

	    if (!success0) {
		    break;
	    }

	    uint8_t crc0[] = { 0xd2, 0x9a, 0xff, 0xc5 };
	    if (!udsdev->routineControl(0x01, 0x02, 0x02, crc0, sizeof(crc0))) {
		    std::cout << "crc0例程控制失败," << udsdev->getLastError() << std::endl;
		    break;
	    }

	    if (!udsdev->ecuReset(0x01)) {
		    std::cout << "ECU复位失败," << udsdev->getLastError() << std::endl;
		    break;
	    }
    } while (false);

    getchar();
    candev->close();
    return 0;
}
```