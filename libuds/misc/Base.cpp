#include "Base.h"

#define UDS_DEBUG_PRINT 0
#if UDS_DEBUG_PRINT
#define UDS_PRINTF printf
#else
#define UDS_PRINTF
#endif

uds::Base::Base(std::shared_ptr<can::Base> base, int channel, int prot, int dlc, 
	CommunicationId cid, GetKeyCallback callback)
	:m_can(base)
{
	m_cid = cid;
	m_getKeyCb = callback;
	m_channel = channel;
	m_dlc = dlc;
	m_proto = static_cast<can::ProtoType>(prot);
	m_frameLen.singleFrame = dlc > 8 ? 6 : 7;
	m_frameLen.firstFrame = dlc > 8 ? (dlc - 6) : (dlc - 2);
	m_frameLen.consecutiveFrame = dlc - 1;
	m_frameLen.flowControlFrame = dlc - 3;
	m_future.tester = std::async(&uds::Base::onTesterPresent, this);
}

uds::Base::~Base()
{
	m_quit = true;
	m_future.tester.wait();
}

std::string uds::Base::getLastError() const
{
	return m_lastError;
}

void uds::Base::setCommunicationId(const uds::CommunicationId& id)
{
	m_cid = id;
}

uds::CommunicationId uds::Base::getCommunicationId() const
{
	return m_cid;
}

void uds::Base::setGetKeyCallback(uds::GetKeyCallback callback)
{
	m_getKeyCb = callback;
}

uds::Buffer uds::Base::getCommunicationBuffer() const
{
	return m_buffer;
}

bool uds::Base::diagnosticSessionControl(uint8_t diagnosticSessionType)
{
	bool result = false;
	do
	{
		size_t size = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
		memset(data.get(), 0x00, size * sizeof(uint8_t));
		data[0] = 0x10;
		data[1] = diagnosticSessionType;
		if (!send(data.get(), 2)) {
			break;
		}

		if (!recv(data.get(), &size)) {
			break;
		}

		if (0x50 == data[0] && diagnosticSessionType == data[1]) {
			m_diagnosticSessionType = diagnosticSessionType;
			setLastError("Diagnostic session successfully");
		}
		else if (0x7f == data[0])
		{
			switch (data[2])
			{
			case 0x12:
				setLastError("Sub function not supported");
				break;

			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;

			case 0x22:
				setLastError("Conditions not correct");
				break;

			default:
				setLastError("Unknown error");
				break;
			}
			insertErrorCode(data[0], data[1], data[2]);
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::ecuReset(uint8_t resetType)
{
	bool result = false;
	do
	{
		size_t size = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
		memset(data.get(), 0x00, size * sizeof(uint8_t));
		data[0] = 0x11;
		data[1] = resetType;

		if (!send(data.get(), 2)) {
			break;
		}

		if (!recv(data.get(), &size)) {
			break;
		}

		if (0x51 == data[0] && resetType == data[1]) {
			if (resetType == RET_ENABLE_RAPID_POWER_SHUTDOWN) {
				auto powerDownTime = data[2];
				if (powerDownTime == 0xff) {
					setLastError("ECU not support enable rapid power shutdown");
					break;
				}
			}
			setLastError("ECU reset successfully");
		}
		else if (0x7f == data[0]) {
			switch (data[2])
			{
			case 0x12:
				setLastError("Sub function not supported");
				break;

			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;

			case 0x22:
				setLastError("Conditions not correct");
				break;

			case 0x33:
				setLastError("Security access denied");
				break;

			default:
				setLastError("Unknown error");
				break;
			}
			insertErrorCode(data[0], data[1], data[2]);
			break;
		}
		else {
			setLastError("Unknown Error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::securityAccess(uint8_t securityAccessType, bool errorKey)
{
	bool result = false;
	do
	{
		size_t size = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
		memset(data.get(), 0x00, size * sizeof(uint8_t));

		data[0] = 0x27;
		data[1] = securityAccessType;
		if (!send(data.get(), 2)) {
			break;
		}

		if (!recv(data.get(), &size)) {
			break;
		}

		if (0x67 != data[0] || securityAccessType != data[1]) {
			setLastError("Get seed failure");
			break;
		}

		//已处于安全解锁模式下，种子返回为0，直接返回true
		bool allZero = true;
		for (size_t i = 0; i < size - 2; i++) {
			if (data[i + 2] != 0) {
				allZero = false;
				break;
			}
		}

		if (allZero/*data[2] == 0 && data[3] == 0 && data[4] == 0  && data[5] == 0*/) {
			return true;
		}

		data[0] = 0x27;
		data[1] = securityAccessType + 0x01;

		uint8_t key[32] = { 0 };
		int keySize = 0;
		if (!errorKey) {
			if (m_getKeyCb) {
				if (!m_getKeyCb(securityAccessType, &data[2], size - 2, key, &keySize)) {
					break;
				}
			}
			else {
				if (!getKey(securityAccessType, &data[2], size - 2, key, &keySize)) {
					break;
				}
			}
		}

		/*
		* @if (keySize) 此处用于解决GetKey之后,密钥长度会发生改变,
		* 将新的长度keySize+2赋值给size
		* @modify 2021.6.8
		*/
		if (keySize) {
			size = keySize + 2;
		}

		for (size_t i = 0; i < size - 2; i++) {
			data[i + 2] = key[i];
		}

		if (!send(data.get(), size)) {
			break;
		}

		size = UDS_BUFFER_SIZE;
		if (!recv(data.get(), &size)) {
			break;
		}

		//positive response
		if (0x67 == data[0] && securityAccessType + 0x01 == data[1]) {
			setLastError("Security access successfully");
		}
		else if (0x7f == data[0]) { //negative response
			switch (data[2])
			{
			case 0x12:
				setLastError("Sub function not Supported");
				break;

			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;

			case 0x22:
				setLastError("Conditions not correct");
				break;

			case 0x24:
				setLastError("Request sequence error");
				break;

			case 0x31:
				setLastError("Request out of range");
				break;

			case 0x35:
				setLastError("Invalid key");
				break;

			case 0x36:
				setLastError("Exceeded number of Attempts");
				break;

			case 0x37:
				setLastError("Required time delay not expired");
				break;

			default:
				setLastError("Unknown error");
				break;
			}
			insertErrorCode(data[0], data[1], data[2]);
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::communicationControl(uint8_t controlType, uint8_t communicationType, uint16_t nodeIdentificationNumber)
{
	bool result = false;
	do
	{
		size_t size = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
		memset(data.get(), 0x00, size * sizeof(uint8_t));
		data[0] = 0x28;
		data[1] = controlType;
		data[2] = communicationType;

		size_t offset = 0;
		if (controlType == uds::CCT_ENABLE_RX_AND_DISABLE_TX_WITH_ENHANCED_ADDRESS_INFORMATION ||
			controlType == uds::CCT_ENABLE_RX_AND_TX_WITH_ENHANCED_ADDRESS_INFORMATION) {
			data[3] = (nodeIdentificationNumber >> 8) & 0xff;
			data[4] = (nodeIdentificationNumber >> 0) & 0xff;
			offset = 2;
		}

		if (!send(data.get(), 3 + offset)) {
			break;
		}

		if (!recv(data.get(), &size)) {
			break;
		}

		if (0x68 == data[0]) {
			setLastError(data[1] == controlType ?
				"Communication control successfully" :
				"Communication control failure");

			if (data[1] != controlType) {
				break;
			}
		}
		else if (0x7f == data[0]) {
			switch (data[2])
			{
			case 0x12:
				setLastError("Sub function not supported");
				break;

			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;

			case 0x22:
				setLastError("Conditions not correct");
				break;

			case 0x31:
				setLastError("Request out of range");
				break;

			default:
				setLastError("Unknown error");
				break;
			}
			insertErrorCode(data[0], data[1], data[2]);
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::testerPresent(uint8_t zeroSubFunction)
{
	bool result = false;
	do 
	{
		size_t size = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
		memset(data.get(), 0x00, size * sizeof(uint8_t));
		data[0] = 0x3e;
		data[1] = zeroSubFunction;
		if (!send(data.get(), 2)) {
			break;
		}

		if (!recv(data.get(), &size)) {
			break;
		}

		if (data[0] == 0x7e) {
			setLastError("Tester present successfully");
		}
		else if (data[0] == 0x7f) {
			switch (data[2])
			{
			case 0x12:
				setLastError("Sub function not supported");
				break;
			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;
			default:
				setLastError("Unknown error");
				break;
			}
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::accessTimingParameter(uint8_t timingParameterAccessType)
{
	bool result = false;
	do
	{
		size_t size = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
		memset(data.get(), 0x00, size * sizeof(uint8_t));
		data[0] = 0x83;
		data[1] = timingParameterAccessType;
		if (!send(data.get(), 2)) {
			break;
		}

		if (!recv(data.get(), &size)) {
			break;
		}

		if (data[0] == 0xc3 && data[1] == timingParameterAccessType) {
			setLastError("Access timing parameter successfully");
		}
		else if (data[0] == 0x7f) {
			switch (data[2])
			{
			case 0x12:
				setLastError("Sub function not supported");
				break;
			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;
			case 0x22:
				setLastError("Conditions not correct");
				break;
			case 0x31:
				setLastError("request out of range");
				break;
			default:
				setLastError("Unknown error");
				break;
			}
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::securedDataTransmission()
{
	bool result = false;
	do
	{
		size_t size = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
		memset(data.get(), 0x00, size * sizeof(uint8_t));
		data[0] = 0x84;
		if (!send(data.get(), 1)) {
			break;
		}

		if (!recv(data.get(), &size)) {
			break;
		}

		if (data[0] == 0xc4) {
			setLastError("Secured data transmission successfully");
		}
		else if (data[0] == 0x7f) {
			if (data[2] == 0x13) {
				setLastError("Incorrect message length or invalid format");
			}
			else if (data[2] >= 0x38 && data[2] <= 0x4f) {
				setLastError("Reserved by extended data link security document");
			}
			else {
				setLastError("Unknown error");
			}
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::controlDtcSetting(uint8_t DtcSettingType)
{
	bool result = false;
	do
	{
		size_t size = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
		memset(data.get(), 0x00, size * sizeof(uint8_t));
		data[0] = 0x85;
		data[1] = DtcSettingType;
		if (!send(data.get(), 2)) {
			break;
		}

		if (!recv(data.get(), &size)) {
			break;
		}

		if (0xc5 == data[0]) {
			setLastError(DtcSettingType == data[1] ? 
				"DTC setting successfully" :
				"DTC setting failure");
		}
		else if (0x7f == data[0]) {
			switch (data[2])
			{
			case 0x12:
				setLastError("Sub function not supported");
				break;

			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;

			case 0x22:
				setLastError("Conditions not correct");
				break;

			case 0x31:
				setLastError("Request out of range");
				break;

			default:
				setLastError("Unknown error");
				break;
			}
			insertErrorCode(data[0], data[1], data[2]);
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::clearDiagnosticInformation()
{
	bool result = false;
	do
	{
		size_t size = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
		memset(data.get(), 0x00, size * sizeof(uint8_t));
		data[0] = 0x14;
		data[1] = 0xff;
		data[2] = 0xff;
		data[3] = 0xff;
		if (!send(data.get(), 4)) {
			break;
		}

		if (!recv(data.get(), &size)) {
			break;
		}

		if (0x54 == data[0]) {
			setLastError("Clear DTC Successfully");
		}
		else if (0x7f == data[0]) {
			switch (data[2])
			{
			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;

			case 0x22:
				setLastError("Conditions not correct");
				break;

			case 0x31:
				setLastError("Request out of range");
				break;

			default:
				setLastError("Unknown error");
				break;
			}
			insertErrorCode(data[0], data[1], data[2]);
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::readDtcInformation(uint8_t reportType, uint8_t statusMask, uint8_t* data, size_t* size)
{
	bool result = false;
	do
	{
		size_t length = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> buffer(new uint8_t[length]);
		memset(buffer.get(), 0, length * sizeof(uint8_t));
		buffer[0] = 0x19;
		buffer[1] = reportType;
		buffer[2] = statusMask;

		if (!send(buffer.get(), 3)) {
			break;
		}

		if (!recv(buffer.get(), &length)) {
			break;
		}

		if (0x59 == buffer[0]) {
			if (*size < length - 3) {
				*size = length - 3;
				setLastError("Buffer too small");
				break;
			}
			memcpy(data, &buffer[3], *size = length - 3);
			setLastError("Read dtc information successfully");
		}
		else if (0x7f == buffer[0]) {
			switch (buffer[2])
			{
			case 0x12:
				setLastError("Sub function not supported");
				break;

			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;

			case 0x22:
				setLastError("Conditions not correct");
				break;

			case 0x31:
				setLastError("Request out of range");
				break;

			default:
				setLastError("Unknown error");
				break;
			}
			insertErrorCode(buffer[0], buffer[1], buffer[2]);
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::readDataByIdentifier(uint8_t dataIdentifierHighBit, uint8_t dataIdentifierLowBit, uint8_t* data, size_t* size)
{
	bool result = false;
	do
	{
		size_t length = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> buffer(new uint8_t[length]);
		memset(buffer.get(), 0, length * sizeof(uint8_t));

		buffer[0] = 0x22;
		buffer[1] = dataIdentifierHighBit;
		buffer[2] = dataIdentifierLowBit;
		if (!send(buffer.get(), 3)) {
			break;
		}

		if (!recv(buffer.get(), &length)) {
			break;
		}

		if (buffer[0] == 0x62 && buffer[1] == dataIdentifierHighBit && buffer[2] == dataIdentifierLowBit) {
			if (*size < length - 3) {
				*size = length - 3;
				setLastError("Buffer to small");
				break;
			}
			memcpy(data, &buffer[3], *size = length - 3);
			setLastError("Read data by identifier successfully");
		}
		else if (0x7f == buffer[0]) {
			switch (buffer[2])
			{
			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;

			case 0x24:
				setLastError("Request sequence error");
				break;

			case 0x31:
				setLastError("Request out of range");
				break;

			default:
				setLastError("Unknown error");
				break;
			}
			insertErrorCode(buffer[0], buffer[1], buffer[2]);
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::writeDataByIdentifier(uint8_t dataIdentifierHighBit, uint8_t dataIdentifierLowBit, const uint8_t* data, size_t size)
{
	bool result = false;
	do
	{
		size_t length = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> buffer(new uint8_t[length]);
		memset(buffer.get(), 0x00, length * sizeof(uint8_t));
		buffer[0] = 0x2e;
		buffer[1] = dataIdentifierHighBit;
		buffer[2] = dataIdentifierLowBit;
		if (size > 0 && nullptr != data) {
			memcpy(&buffer[3], data, size);
		}

		if (!send(buffer.get(), 3 + size)) {
			break;
		}

		if (!recv(buffer.get(), &length)) {
			break;
		}

		if (buffer[0] == 0x6e && buffer[1] == dataIdentifierHighBit && buffer[2] == dataIdentifierLowBit) {
			setLastError("Write data by identifier successfully");
		}
		else if (buffer[0] == 0x7f) {
			switch (buffer[2])
			{
			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;

			case 0x22:
				setLastError("Conditions not correct");
				break;

			case 0x31:
				setLastError("Request out of range");
				break;

			case 0x33:
				setLastError("Security access denied");
				break;

			case 0x72:
				setLastError("General programming failure");
				break;

			default:
				setLastError("Unknown error");
				break;
			}
			insertErrorCode(buffer[0], buffer[1], buffer[2]);
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::routineControl(uint8_t routineControlType, uint8_t routineIdentifierHighBit, uint8_t routineIdentifierLowBit, const uint8_t* data, size_t size)
{
	bool result = false;
	do
	{
		size_t length = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> buffer(new uint8_t[length]);
		memset(buffer.get(), 0x00, length * sizeof(uint8_t));
		buffer[0] = 0x31;
		buffer[1] = routineControlType;
		buffer[2] = routineIdentifierHighBit;
		buffer[3] = routineIdentifierLowBit;
		if (data != nullptr && size > 0) {
			memcpy(&buffer[4], data, size);
		}

		if (!send(buffer.get(), 4 + size)) {
			break;
		}

		if (!recv(buffer.get(), &length)) {
			break;
		}

		if (buffer[0] == 0x71 && buffer[1] == routineControlType &&
			buffer[2] == routineIdentifierHighBit &&
			buffer[3] == routineIdentifierLowBit) {
			setLastError("Routine control successfully");
		}
		else if (buffer[0] == 0x7f) {
			switch (buffer[2])
			{
			case 0x12:
				setLastError("Sub function not supported");
				break;

			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;

			case 0x22:
				setLastError("Conditions not correct");
				break;

			case 0x24:
				setLastError("Request sequence error");
				break;

			case 0x31:
				setLastError("Request out of range");
				break;

			case 0x33:
				setLastError("Security access denied");
				break;

			case 0x72:
				setLastError("General programming failure");
				break;

			default:
				setLastError("Unknown error");
				break;
			}
			insertErrorCode(buffer[0], buffer[1], buffer[2]);
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::requestDownload(uint8_t dataFormatIdentifier, uint8_t addressAndLengthFormatIdentifier, uint32_t memoryAddress, size_t memorySize)
{
	bool result = false;
	do
	{
		size_t size = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
		memset(data.get(), 0x00, size * sizeof(uint8_t));
		data[0] = 0x34;
		data[1] = dataFormatIdentifier;
		data[2] = addressAndLengthFormatIdentifier;
		data[3] = static_cast<uint8_t>(memoryAddress >> 24);
		data[4] = static_cast<uint8_t>(memoryAddress >> 16);
		data[5] = static_cast<uint8_t>(memoryAddress >> 8);
		data[6] = static_cast<uint8_t>(memoryAddress >> 0);
		data[7] = static_cast<uint8_t>(memorySize >> 24);
		data[8] = static_cast<uint8_t>(memorySize >> 16);
		data[9] = static_cast<uint8_t>(memorySize >> 8);
		data[10] = static_cast<uint8_t>(memorySize >> 0);

		if (!send(data.get(), 11)) {
			break;
		}

		if (!recv(data.get(), &size)) {
			break;
		}

		if (0x74 == data[0]) {
			setLastError("Request Download Successfully");
			size_t lengthFormatIdentifier = (data[1] & 0xf0);
			size_t maxNumberOfBlockLength = 0;
			for (size_t i = 0; i < lengthFormatIdentifier; ++i) {
				maxNumberOfBlockLength +=
					((size_t)(data[i + 2] << ((lengthFormatIdentifier - 1 - i) * 8)));
			}
			m_maxNumberOfBlockLength = maxNumberOfBlockLength;
		}
		else if (0x7f == data[0]) {
			switch (data[2])
			{
			case 0x13:
				setLastError("Incorrect Message Length Of Invalid Format");
				break;

			case 0x22:
				setLastError("Conditions Not Correct");
				break;

			case 0x31:
				setLastError("Request Out of Range");
				break;

			case 0x33:
				setLastError("Security Access Denied");
				break;

			case 0x70:
				setLastError("Server Error,Upload/Download Is Denied");
				break;

			default:
				setLastError("Unknown Error");
				break;
			}
			insertErrorCode(data[0], data[1], data[2]);
			break;
		}
		else {
			setLastError("Unknown Error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::transferData(uint8_t blockSequenceCounter, const uint8_t* data, size_t size)
{
	bool result = false;
	do
	{
		if (m_maxNumberOfBlockLength < size) {
			setLastError("数据大小错误");
			break;
		}

		size_t length = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> buffer(new uint8_t[length]);
		buffer[0] = 0x36;
		buffer[1] = blockSequenceCounter;
		memcpy(&buffer[2], data, size);

		if (!send(buffer.get(), 2 + size)) {
			break;
		}

		if (!recv(buffer.get(), &length)) {
			break;
		}

		if (buffer[0] == 0x76 && buffer[1] == blockSequenceCounter) {
			setLastError("Transfer Data Successfully");
		}
		else if (0x7f == buffer[0]) {
			switch (buffer[2])
			{
			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;

			case 0x24:
				setLastError("Request sequence error");
				break;

			case 0x31:
				setLastError("Request out of range");
				break;

			case 0x71:
				setLastError("Transfer data suspended");
				break;

			case 0x72:
				setLastError("General programming failure");
				break;

			case 0x73:
				setLastError("Wrong block sequence counter");
				break;

			default:
				setLastError("Unknown error");
				break;
			}

			insertErrorCode(buffer[0], buffer[1], buffer[2]);
			if (buffer[2] != 0x78) {
				break;
			}
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::inputOutputControlByIdentifier(uint8_t dataIdentifierHighBit, uint8_t dataIdentifierLowBit, uint8_t ioControlParameter, uint8_t* controlEnableMaskRecord, size_t controlEnableMaskRecordSize)
{
	bool result = false;
	do
	{
		size_t size = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
		memset(data.get(), 0x00, size * sizeof(uint8_t));
		data[0] = 0x2f;
		data[1] = dataIdentifierHighBit;
		data[2] = dataIdentifierLowBit;
		data[3] = ioControlParameter;
		size_t offset = 0;
		if (controlEnableMaskRecord) {
			memcpy(&data[4], controlEnableMaskRecord, controlEnableMaskRecordSize);
			offset = controlEnableMaskRecordSize;
		}

		if (!send(data.get(), 4 + offset)) {
			break;
		}

		if (!recv(data.get(), &size)) {
			break;
		}

		if (0x6f == data[0] && dataIdentifierHighBit == data[1] && dataIdentifierLowBit == data[2]) {
			setLastError("Input output control by identifier successfully");
		}
		else if (0x7f == data[0])
		{
			switch (data[2])
			{
			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;

			case 0x22:
				setLastError("Conditions not correct");
				break;

			case 0x31:
				setLastError("Request out of range");
				break;

			case 0x33:
				setLastError("Security access denied");
				break;

			case 0x24:
				setLastError("Request sequence error");
				break;

			default:
				setLastError("Unknown error");
				break;
			}
			insertErrorCode(data[0], data[1], data[2]);
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
	return false;
}

bool uds::Base::requestTransferExit()
{
	bool result = false;
	do
	{
		size_t size = UDS_BUFFER_SIZE;
		std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
		memset(data.get(), 0x00, size * sizeof(uint8_t));
		data[0] = 0x37;
		if (!send(data.get(), 1)) {
			break;
		}

		if (!recv(data.get(), &size)) {
			break;
		}

		if (0x77 == data[0]) {
			setLastError("Request transfer exit successfully");
		}
		else if (0x7f == data[0])
		{
			switch (data[2])
			{
			case 0x13:
				setLastError("Incorrect message length or invalid format");
				break;

			case 0x22:
				setLastError("Conditions not correct");
				break;

			case 0x24:
				setLastError("Request sequence error");
				break;

			default:
				setLastError("Unknown error");
				break;
			}
			insertErrorCode(data[0], data[1], data[2]);
			break;
		}
		else {
			setLastError("Unknown error");
			break;
		}

		result = true;
	} while (false);
	return result;
}

void uds::Base::startTesterPresent(int period)
{
	m_testerPresentStart = true;
	m_testerPresentPeriod = period;
}

void uds::Base::stopTesterPresent()
{
	m_testerPresentStart = false;
}

void uds::Base::setRecvTimeout(int timeout)
{
	m_recvTimeout = timeout;
}

bool uds::Base::send(const uint8_t* data, size_t size)
{
	bool result = false, success = false;
	do
	{
		copyToBuffer(0, data, size);
		auto status = CS_WAITTING_START;
		auto sn = 1, fcStatus = 0, blockSize = 0, stMin = 0;
		do
		{
			switch (status)
			{
			case CS_WAITTING_START:
				if (size <= m_frameLen.singleFrame && size > 0) {
					status = CS_SEND_SF;
				}
				else if (size > 0 && size < UDS_BUFFER_SIZE) {
					status = CS_SEND_FF;
				}
				else {
					status = CS_FINISH;
					success = false;
				}
				break;

				//发送单帧
			case CS_SEND_SF:
				if (sendSf(data, size)) {
					success = true;
					status = CS_FINISH;
				}
				else {
					setLastError("发送单帧失败");
					status = CS_FINISH;
					success = false;
				}
				break;

				//发送首帧
			case CS_SEND_FF:
				if (sendFf(data, size)) {
					data += m_frameLen.firstFrame;
					size -= m_frameLen.firstFrame;
					status = CS_RECV_FC;
				}
				else {
					setLastError("发送首帧失败");
					status = CS_FINISH;
					success = false;
				}
				break;

				//发送连续帧
			case CS_SEND_CF:
				if (blockSize > 0) {
					if (sendCf(data, sn)) {
						data += m_frameLen.consecutiveFrame;
						size -= m_frameLen.consecutiveFrame;
						if ((int)size <= 0) {
							success = true;
							status = CS_FINISH;
						}
						++sn;
						--blockSize;
					}
					else {
						setLastError("发送连续帧失败");
						status = CS_FINISH;
						success = false;
					}
				}
				else {
					status = CS_RECV_FC;
				}
				break;

			case CS_SEND_FC:
			case CS_RECV_SF:
			case CS_RECV_FF:
			case CS_RECV_CF:
				setLastError("发送诊断逻辑错误");
				status = CS_FINISH;
				success = false;
				break;

				//接收流控帧
			case CS_RECV_FC:
				if (recvFc(&fcStatus, &blockSize, &stMin)) {
					auto overflow = false;
					switch (fcStatus)
					{
					case uds::FS_CONTINUE_TO_SEND:
						/*
						* @brief blockSize
						* 0x00 -> 接收方向发送端告知: 接收端后续将不会再发送流控帧响应，发送端也不需要等待流控帧，发送端直接发送余下的连接帧数据；
						* 0x01~0xFF -> 接收方向发送端告知：接收端能接收的连续帧的数量，单位：数据帧；
						*/
						if (blockSize == 0) {
							blockSize = 0xff;
							status = CS_SEND_CF;
						}
						else if (blockSize > 0) {
							status = CS_SEND_CF;
						}

						//微秒
						if ((stMin & 0xf0) == 0xf0 &&
							(stMin & 0x0f) != 0x00) {
							if ((stMin & 0x0f) >= 0x01 &&
								(stMin & 0x0f) <= 0x09) {
								stMin &= 0x0f;
								stMin *= 100;
								stMin /= 1000;//精度不够
							}
							else {
								//预留
							}
						}
						else if (stMin >= 0x80 && stMin <= 0xf0) {
							//预留
						}
						break;
					case uds::FS_WAITTING:
						//等待
						break;
					case uds::FS_OVERFLOW:
						overflow = true;
						break;
					default:
						break;
					}

					if (overflow) {
						status = CS_FINISH;
						success = false;
						setLastError("接收流控帧溢出");
					}
				}
				else {
					status = CS_FINISH;
					success = false;
					setLastError("接收流控帧失败");
				}
				break;

			default:
				status = CS_FINISH;
				success = false;
				break;
			}

			m_timer.getStartTime();
			while (true)
			{
				if (m_timer.getEndTime() > stMin) {
					break;
				}
			}
		} while (CS_FINISH != status);

		if (!success) {
			break;
		}
		result = true;
	} while (false);
	return result;
}

bool uds::Base::sendSf(const uint8_t* data, size_t size)
{
	bool result = false;
	do
	{
		can::Msg msg;
		msg.id = getRequestId();
		msg.dlc = m_dlc;
		msg.protoType = m_proto;
		if (m_dlc > 8) { //CANFD
			if (size > m_dlc - 2) {
				break;
			}
			msg.data[0] = (FT_SF << 4);
			msg.data[1] = (size & 0xff);
			memcpy(&msg.data[2], data, size);
		}
		else { //CAN
			if (size > m_dlc - 1) {
				break;
			}
			msg.data[0] = ((FT_SF << 4) | (size & 0x0f));
			memcpy(&msg.data[1], data, size);
		}

		if (!m_can->sendMsg(&msg, 1, m_channel)) {
			break;
		}
		result = true;
	} while (false);
	return result;
}

bool uds::Base::sendFf(const uint8_t* data, size_t size)
{
	bool result = false;
	do
	{
		can::Msg msg;
		msg.id = getRequestId();
		msg.dlc = m_dlc;
		msg.protoType = m_proto;
		if (size > 4096) { //CANFD
			msg.data[0] = (FT_FF << 4);
			msg.data[1] = 0x00;
			msg.data[2] = ((size >> 24) & 0xff);
			msg.data[3] = ((size >> 16) & 0xff);
			msg.data[4] = ((size >> 8) & 0xff);
			msg.data[5] = ((size >> 0) & 0xff);
			memcpy(&msg.data[6], data, m_frameLen.firstFrame);
		}
		else { //CAN
			msg.data[0] = ((FT_FF << 4) | ((size >> 8) & 0x0f));
			msg.data[1] = size & 0xff;
			memcpy(&msg.data[2], data, m_frameLen.firstFrame);
		}

		if (!m_can->sendMsg(&msg, 1, m_channel)) {
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::sendCf(const uint8_t* data, size_t sn)
{
	bool result = false;
	do
	{
		can::Msg msg;
		msg.id = getRequestId();
		msg.dlc = m_dlc;
		msg.protoType = m_proto;
		msg.data[0] = ((FT_CF << 4) | ((sn % 16) & 0x0f));
		memcpy(&msg.data[1], data, m_frameLen.consecutiveFrame);
		if (!m_can->sendMsg(&msg, 1, m_channel)) {
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::sendFc()
{
	bool result = false;
	do
	{
		can::Msg msg;
		msg.id = getRequestId();
		msg.dlc = m_dlc;
		msg.protoType = m_proto;
		msg.data[0] = ((FT_FC << 4) | (0 & 0x0f));
		msg.data[1] = 0xff;
		msg.data[2] = 0x00;

		if (!m_can->sendMsg(&msg, 1, m_channel)) {
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool uds::Base::recv(uint8_t* data, size_t* size)
{
	bool result = false, overflow = false, quit = false, timeout = false;
	do
	{
		int sn = 1, blockSize = 0xff;
		std::unique_ptr<can::Msg[]> msg(new can::Msg[CAN_MAX_RECV_BUFFER_SIZE]);

		Timer timer;
		timer.getStartTime();

		size_t count = 0;
		do
		{
			auto ret = m_can->recvMsg(msg.get(), CAN_MAX_RECV_BUFFER_SIZE, m_channel);
			for (size_t i = 0; i < ret; ++i) {
				if (m_cid.response == msg[i].id) {
					if (0x02 == msg[i].data[0] && 0x7e == msg[i].data[1] && 0x00 == msg[i].data[2]) {
						continue;
					}
					else if (0x03 == msg[i].data[0] && 0x7f == msg[i].data[1] && 0x78 == msg[i].data[3]) {
						continue;
					}

					//if (0x03 == msg[i].data[0] && 0x7f == msg[i].data[1] && 0x3e == msg[i].data[2]) {
					//	count = msg[i].data[0] & 0x0f;
					//	if (count > *size) {
					//		overflow = true;
					//	}
					//	else {
					//		*size = count;
					//		memcpy(data, &msg[i].data[1], *size);
					//		setLastError("TesterPresent错误");
					//	}
					//	quit = true;
					//	break;
					//}

					timer.getStartTime();

					auto frameType = ((msg[i].data[0] & 0xf0) >> 4);
					switch (frameType)
					{
					case FT_SF:
						UDS_PRINTF("recv single frame\n");
						count = msg[i].data[0] & 0x0f;
						if (count != 0) { //CAN
							if (count > *size) {
								overflow = true;
							}
							else {
								memcpy(data, &msg[i].data[1], *size = count);
							}
						}
						else { //CANFD
							count = msg[i].data[1];
							if (count > *size) {
								overflow = true;
							}
							else {
								memcpy(data, &msg[i].data[2], *size = count);
							}
						}
						quit = true;
						break;

					case FT_FF:
						UDS_PRINTF("recv first frame\n");
						count = ((msg[i].data[0] & 0x0f) << 8) + msg[i].data[1];
						if (count > *size) {
							overflow = true;
							quit = true;
						}
						else {
							*size = count;
							if (count > 0) { //CAN
								memcpy(data, &msg[i].data[2], m_frameLen.firstFrame);
							}
							else { //CANFD
								count = (msg[i].data[2] << 24) + (msg[i].data[3] << 16) + (msg[i].data[4] << 8) + (msg[i].data[5]);
								memcpy(data, &msg[i].data[6], m_frameLen.firstFrame);
							}

							if (!sendFc()) {
								setLastError("发送流控帧失败");
								quit = true;
							}
						}
						break;

					case FT_CF:
						UDS_PRINTF("recv consecutive frame\n");
						if (sn % 16 != (msg[i].data[0] & 0x0f)) {
							setLastError("连续帧序列号错误");
							quit = true;
						}
						else {
							auto index = m_frameLen.firstFrame + (sn - 1) * m_frameLen.consecutiveFrame;
							memcpy(&data[index], &msg[i].data[1], m_frameLen.consecutiveFrame);
							if ((++sn - 1) * m_frameLen.consecutiveFrame >= *size - m_frameLen.firstFrame) {
								quit = true;
								UDS_PRINTF("recv consecutive frame end\n");
							}
							else {
								if (--blockSize == 0) {
									UDS_PRINTF("send flow control frame\n");
									if (!sendFc()) {
										setLastError("发送流控帧失败");
										quit = true;
									}
									blockSize = 0xff;
								}
							}
						}
						break;

					case FT_FC:
						UDS_PRINTF("recv flow control\n");
						setLastError("接收诊断逻辑错误");
						quit = true;
						break;
					}

					if (quit) {
						break;
					}

					Sleep(1);
				}
			}

			if (timer.getEndTime() > m_recvTimeout) {
				timeout = true;
				break;
			}
		} while (!quit);

		if (timeout) {
			setLastError("接收诊断报文超时");
			break;
		}

		if (overflow) {
			setLastError("接收缓冲区溢出");
			break;
		}

		copyToBuffer(1, data, *size);
		result = true;
	} while (false);
	return result;
}

bool uds::Base::recvFc(int* fcStatus, int* blockSize, int* stMin)
{
	bool result = false, find = false;
	do
	{
		std::unique_ptr<can::Msg[]> msg(new can::Msg[CAN_MAX_RECV_BUFFER_SIZE]);

		while (true) {
			auto size = m_can->recvMsg(msg.get(), CAN_MAX_RECV_BUFFER_SIZE, m_channel);
			for (size_t i = 0; i < size; ++i) {
				if (m_cid.response != msg[i].id) {
					continue;
				}

				auto frameType = ((msg[i].data[0] & 0xf0) >> 4);
				if (frameType == FT_FC) {
					*fcStatus = (msg[i].data[0] & 0x0f);
					*blockSize = msg[i].data[1];
					*stMin = msg[i].data[2];
					find = true;
					break;
				}
			}

			if (find) {
				break;
			}

			if (m_timer.getEndTime() > 140) {
				break;
			}
		}

		if (!find) {
			break;
		}

		result = true;
	} while (false);
	return result;
}

void uds::Base::setLastError(const char* fmt, ...)
{
	va_list ap = nullptr;
	va_start(ap, fmt);
	auto length = _vscprintf_p(fmt, ap) + 1;
	std::unique_ptr<char[]> buffer(new char[length]);
	memset(buffer.get(), 0, length);
	_vsprintf_p(buffer.get(), length, fmt, ap);
	va_end(ap);
	m_lastError = buffer.get();
}

void uds::Base::insertErrorCode(uint8_t id0, uint8_t id1, uint8_t id2)
{
	char temp[32] = { 0 };
	sprintf_s(temp, "%02x %02x %02x,", id0, id1, id2);
	m_lastError.insert(0, temp);
}

bool uds::Base::getKey(int level, const uint8_t* seedData, int seedSize, uint8_t* keyData, int* keySize)
{
	setLastError("子类未实现getKey函数");
	return false;
}

int uds::Base::getRequestId() const
{
	return m_cid.request.mode == RM_PHYSICAL ? m_cid.request.physical : m_cid.request.function;
}

int uds::Base::getResponseId() const
{
	return m_cid.response;
}

void uds::Base::copyToBuffer(int flag, const uint8_t* data, size_t size)
{
	if (size <= UDS_BUFFER_SIZE) {
		if (flag == 0) {
			memcpy(m_buffer.send.data, data, size);
		}
		else {
			memcpy(m_buffer.recv.data, data, size);
		}
	}
}

void uds::Base::onTesterPresent()
{
	can::Msg msg;
	msg.id = getRequestId();
	msg.data[0] = 0x02;
	msg.data[1] = 0x3e;
	msg.data[2] = 0x80;

	auto startTime = GetTickCount64();
	while (!m_quit) {
		auto endTime = GetTickCount64();
		if (m_testerPresentStart) {
			if (endTime - startTime >= 1000) {
				startTime = endTime;
				switch (m_diagnosticSessionType)
				{
				case uds::DST_DEFAULT:
					break;
				case uds::DST_PROGRAMMING:
				case uds::DST_EXTEND:
					if (m_can != nullptr && m_can->isOpen()) {
						m_can->sendMsg(&msg, 1, m_channel);
					}
					break;
				default:
					break;
				}
			}
		}
		else {
			startTime = endTime;
		}
		Sleep(10);
	}
}
