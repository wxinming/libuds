# libuds
UDS类库
```

// 此代码演示如何将bin文件下载到ECU中

#include <libcan/libcan.h>
#include <libuds/libuds.h>
#ifdef _DEBUG
#pragma comment(lib, "libcand.lib")
#pragma comment(lib, "libudsd.lib")
#else
#pragma comment(lib, "libcan.lib")
#pragma comment(lib, "libuds.lib")
#endif

static bool readFile(const std::string& filename, std::unique_ptr<uint8_t[]>& buffer, size_t& size)
{
	FILE* fp = fopen(filename.c_str(), "rb");
	if (!fp) {
		std::cout << __LINE__ << "->打开" << filename << "失败\n";
		return false;
	}

	fseek(fp, SEEK_SET, SEEK_END);
	size = ftell(fp);
	fseek(fp, SEEK_SET, SEEK_SET);

	buffer = std::make_unique<uint8_t[]>(size);
	fread(buffer.get(), 1, size, fp);
	fclose(fp);

	return true;
}

static bool downloadFile(const std::string& filename, std::shared_ptr<uds::Base> udssp, uint32_t memoryAddress, size_t memorySize)
{
	bool result = false;
	do 
	{
		if (!udssp->requestDownload(0x00, 0x44, memoryAddress, memorySize)) {
			std::cout << __LINE__ << "->请求下载失败," << udssp->getLastError() << std::endl;
			break;
		}

		std::unique_ptr<uint8_t[]> buffer;
		size_t size = 0;
		if (!readFile(filename, buffer, size)) {
			break;
		}

		size -= 9;
		uint32_t block = 0;
		auto success = true;
		for (size_t i = 0; i < size; i += 256) {
			auto written = size - i > 256 ? 256 : size - i;
			if (!udssp->transferData(++block % 256, &buffer[i + 9], written)) {
				success = false;
				std::cout << __LINE__ << "->传输数据失败," << udssp->getLastError() << std::endl;
				break;
			}
		}

		if (!success) {
			break;
		}

		if (!udssp->requestTransferExit()) {
			std::cout << __LINE__ << "->请求传输退出失败," << udssp->getLastError() << std::endl;
			break;
		}

		if (memoryAddress == 0x00) {
			uint8_t crc[] = { 0xd2, 0x9a, 0xff, 0xc5 };
			if (!udssp->routineControl(0x01, 0x02, 0x02, crc, sizeof(crc))) {
				std::cout << __LINE__ << "->例程控制失败," << udssp->getLastError() << std::endl;
				break;
			}

			Sleep(50);
			
			uint8_t data[] = { 0x44, 0x00, 0x00, 0xa0, 0x00, 0x00, 0x01, 0x06, 0x00 };
			if (!udssp->routineControl(0x01, 0xff, 0x00, data, sizeof(data))) {
				std::cout << __LINE__ << "->例程控制失败," << udssp->getLastError() << std::endl;
				break;
			}

			std::cout << __LINE__ << "->下载" << filename << "完成!" << std::endl;
		}
		else {
			uint8_t crc[] = { 0xf4, 0xc1, 0x13, 0xb5 };
			if (!udssp->routineControl(0x01, 0x02, 0x02, crc, sizeof(crc))) {
				std::cout << __LINE__ << "->例程控制失败," << udssp->getLastError() << std::endl;
				break;
			}

			Sleep(50);

			if (!udssp->routineControl(0x01, 0xff, 0x01, nullptr, 0)) {
				std::cout << __LINE__ << "->例程控制失败," << udssp->getLastError() << std::endl;
				break;
			}
		}
		result = true;
	} while (false);
	return result;
}

int main(int argc, char *argv[])
{
	auto cansp = can::autoReleaseNew(can::ZLG_USBCANFD200U);
	
	// 设置需要过滤的CAN ID
	cansp->setFilterId({ 0x727, 0x7a7, 0x7df });

	// 设置调试控制台输出CAN日志
	cansp->setOutputLog(true);

	// 注册CAN报文处理函数
	cansp->setMsgProc([](const char* direction, const can::Msg& msg) {
		std::cout << direction << "->" << msg << std::endl;
	});

	uds::CommunicationId cid = { 0 };
	
	// 设置默认请求模式为物理请求
	cid.request.mode = uds::RequestMode::PHYSICAL;
	
	// 设置物理请求ID
	cid.request.physical = 0x727;
	
	// 设置功能请求ID
	cid.request.function = 0x7df;
	
	// 设置响应ID
	cid.response = 0x7a7;
	
	// 安全访问解锁函数
	auto getKeyFunc = [&](int level, const unsigned char* seedData, int seedSize,
		unsigned char* keyData, int* keySize)->bool {
		unsigned int mask = level == 0x11 ? 0x12345678 : 0x87654321;
		unsigned int seed = 0;
		unsigned int key = 0;

		for (int i = 0; i < 4; ++i) {
			seed |= static_cast<unsigned int>(seedData[i] << 8 * (3 - i));
		}

		for (int i = 0; i < 35; ++i) {
			if (seed & 0x80000000) {
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

	auto udssp = uds::autoReleaseNew(cansp, 0, can::CANFD, cid, getKeyFunc);

	can::Device dev; // 默认会打开索引0,通道0
	if (!cansp->open(dev)) {
		std::cout << __LINE__ << "->打开CAN卡失败," << cansp->getLastError() << std::endl;
		return -1;
	}

	do
	{
		Sleep(3000); // 防止一打开CAN没有报文导致发送或接收超时
#if 0
		{
			uint8_t data[256] = { 0 };
			size_t size = sizeof(data) / sizeof(uint8_t);
			if (!udssp->readDataByIdentifier(0x3a, 0x0c, data, &size)) {
				std::cout << __LINE__ << "->通过标识符读取数据失败," << udssp->getLastError() << std::endl;
				break;
			}
			std::cout << __LINE__ << "->0x3a0c:" << std::string(reinterpret_cast<const char*>(data), size) << std::endl;
			break;
		}
#endif

		udssp->setRequestMode(uds::RequestMode::FUNCTIONAL);
		if (!udssp->diagnosticSessionControl(0x03)) {
			std::cout << __LINE__ << "->诊断会话控制失败," << udssp->getLastError() << std::endl;
			break;
		}

		udssp->setRequestMode(uds::RequestMode::PHYSICAL);
		if (!udssp->routineControl(0x01, 0x02, 0x03, nullptr, 0)) {
			std::cout << __LINE__ << "->例程控制失败," << udssp->getLastError() << std::endl;
			break;
		}

		udssp->setRequestMode(uds::RequestMode::FUNCTIONAL);
		if (!udssp->controlDtcSetting(0x02)) {
			std::cout << __LINE__ << "->控制DTC设置失败," << udssp->getLastError() << std::endl;
			break;
		}

		udssp->setRequestMode(uds::RequestMode::PHYSICAL);
		if (!udssp->communicationControl(0x03, 0x01)) {
			std::cout << __LINE__ << "->通讯控制设置失败," << udssp->getLastError() << std::endl;
			break;
		}

		udssp->setRequestMode(uds::RequestMode::FUNCTIONAL);
		if (!udssp->communicationControl(0x03, 0x01)) {
			std::cout << __LINE__ << "->通讯控制置失败," << udssp->getLastError() << std::endl;
			break;
		}

		udssp->setRequestMode(uds::RequestMode::PHYSICAL);
		if (!udssp->diagnosticSessionControl(0x02)) {
			std::cout << __LINE__ << "->诊断会话控制失败," << udssp->getLastError() << std::endl;
			break;
		}
		Sleep(1000);

		if (!udssp->securityAccess(0x11)) {
			std::cout << __LINE__ << "->安全访问失败," << udssp->getLastError() << std::endl;
			break;
		}

		uint8_t data[] = { 0x25, 0x07, 0x31, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f };
		if (!udssp->writeDataByIdentifier(0xf1, 0x84, data, sizeof(data))) {
			std::cout << __LINE__ << "->通过标识符写数据失败," << udssp->getLastError() << std::endl;
			break;
		}

		if (!downloadFile("1-AVM_A512E01905_V001_2.bin", udssp, 0x00, 0x0c04)) {
			break;
		}

		if (!downloadFile("processed_AVM_B30X_S32K116_APP_5.bin", udssp, 0x0000a000, 0x00016000)) {
			break;
		}

		if (!udssp->ecuReset(0x01)) {
			std::cout << __LINE__ << "->ECU复位失败," << udssp->getLastError() << std::endl;
			break;
		}

		Sleep(120);

		if (!udssp->diagnosticSessionControl(0x03)) {
			std::cout << __LINE__ << "->诊断会话控制失败," << udssp->getLastError() << std::endl;
			break;
		}

		{
			uint8_t data[256] = { 0 };
			size_t size = sizeof(data) / sizeof(uint8_t);
			if (!udssp->readDataByIdentifier(0x3a, 0x0c, data, &size)) {
				std::cout << __LINE__ << "->通过标识符读取数据失败," << udssp->getLastError() << std::endl;
				break;
			}
			std::cout << __LINE__ << "->0x3a0c:" << std::string(reinterpret_cast<const char*>(data), size) << std::endl;
		}

	} while (false);
	cansp->close();
	return 0;
}
```