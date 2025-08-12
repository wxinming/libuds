#pragma once
#include "Type.h"
#include "../../../libcan/libcan/libcan.h"
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "../libcan/x64/Debug/libcand.lib")
#else
#pragma comment(lib, "../libcan/x64/Release/libcan.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "../libcan/Win32/Debug/libcand.lib")
#else
#pragma comment(lib, "../libcan/Win32/Release/libcan.lib")
#endif
#endif

namespace uds {
	class LIBUDS_DLL_EXPORT Base 
	{
	public:
		/*
		* @brief 构造
		* @param[in] base can卡类型
		* @param[in] channel can卡通道
		* @param[in] proto 协议类型
		* @param[in] cid 通讯id
		* @param[in] callback 获取key回调可以为nullptr
		*/
		Base(std::shared_ptr<can::Base> base, int channel = 0, can::ProtoType proto = can::ProtoType::CAN, CommunicationId cid = { 0 }, GetKeyCallback callback = nullptr);

		/*
		* @brief 析构
		*/
		virtual ~Base();

		/*
		* @brief 拷贝构造删除
		*/
		Base(const uds::Base&) = delete;

		/*
		* @brief 赋值构造删除
		*/
		Base& operator=(const uds::Base&) = delete;

		/*
		* @brief 获取最终错误
		* @return std::string
		*/
		std::string getLastError() const;

		/*
		* @brief 设置通讯ID
		* @param[in] id id
		* @return void
		*/
		void setCommunicationId(const uds::CommunicationId& id);

		/*
		* @brief 获取通讯ID
		* @return uds::CommunicationId
		*/
		uds::CommunicationId getCommunicationId() const;

		/*
		* @brief 设置获取密钥回调
		* @param[in] callback 回调
		* @return void
		*/
		void setGetKeyCallback(uds::GetKeyCallback callback);

		/*
		* @brief 获取通讯缓冲区
		* @return uds::Buffer
		*/
		uds::Buffer getCommunicationBuffer() const;

		/*
		* @brief 诊断会话控制
		* @param[in] diagnosticSessionType 诊断会话类型
		* @return bool
		*/
		bool diagnosticSessionControl(uint8_t diagnosticSessionType);

		/*
		* @brief 控制器重置
		* @param[in] resetType 重置类型
		* @return bool
		*/
		bool ecuReset(uint8_t resetType);

		/*
		* @brief 安全访问
		* @param[in] securityAccessType 安全访问类型
		* @param[in] errorKey 是否为错误密钥
		* @return bool
		*/
		bool securityAccess(uint8_t securityAccessType, bool errorKey = false);

		/*
		* @brief 通信控制
		* @param[in] controlType 控制类型
		* @param[in] communicationType 通讯类型
		* @param[in] nodeIdentificationNumber 节点识别号
		* @return bool
		* @note 只有当communicationType为4或5时,nodeIdentificationNumber才有意义
		*/
		bool communicationControl(uint8_t controlType, uint8_t communicationType, uint16_t nodeIdentificationNumber = 0);

		/*
		* @brief 保持连接
		* @param[in] zeroSubFunction 零子功能
		* @return bool
		*/
		bool testerPresent(uint8_t zeroSubFunction);

		/*
		* @brief 访问定时参数
		* @param[in] timingParameterAccessType 定时参数访问类型
		* @return bool
		*/
		bool accessTimingParameter(uint8_t timingParameterAccessType);

		/*
		* @brief 安全数据传输
		* @return bool
		*/
		bool securedDataTransmission();

		/*
		* @brief 控制DTC设置
		* @param[in] DtcSettingType 诊断故障码设置类型
		* @return bool
		*/
		bool controlDtcSetting(uint8_t DtcSettingType);

		/*
		* @brief 清除诊断信息
		* @return bool
		*/
		bool clearDiagnosticInformation();

		/*
		* @brief 读取Dtc信息
		* @param[in] reportType 报告类型
		* @param[in] statusMask 状态掩码
		* @param[out] data 获取的数据
		* @param[in|out] size 获取的大小
		* @return bool
		*/
		bool readDtcInformation(uint8_t reportType, uint8_t statusMask, uint8_t* data, size_t* size);

		/*
		* @brief 通过标识符读取数据
		* @param[in] dataIdentifierHighBit 数据标识高位
		* @param[in] dataIdentifierLowBit 数据标识低位
		* @param[in] data 获取的数据
		* @param[in|out] size 获取的大小
		* @return bool
		*/
		bool readDataByIdentifier(uint8_t dataIdentifierHighBit, uint8_t dataIdentifierLowBit, uint8_t* data, size_t* size);

		/*
		* @brief 通过标识符写入数据
		* @param[in] dataIdentifierHighBit 数据标识高位
		* @param[in] dataIdentifierLowBit 数据标识低位
		* @param[in] data 需要写入的数据
		* @param[in] size 写入的数据大小
		* @return bool
		*/
		bool writeDataByIdentifier(uint8_t dataIdentifierHighBit, uint8_t dataIdentifierLowBit, const uint8_t* data, size_t size);

		/*
		* @brief 例程控制
		* @param[in] routineControlType 例程控制类型
		* @param[in] routineIdentifierHighBit 例程标识符高位
		* @param[in] routineIdentifierLowBit 例程标识符低位
		* @param[in] data 需要发送的数据
		* @param[in] size 发送数据的大小
		* @return bool
		*/
		bool routineControl(uint8_t routineControlType, uint8_t routineIdentifierHighBit, uint8_t routineIdentifierLowBit, const uint8_t* data, size_t size);

		/*
		* @brief 请求下载
		* @param[in] dataFormatIdentifier 数据格式标识符
		* @param[in] addressAndLengthFormatIdentifier 地址和长度格式标识符
		* @param[in] memoryAddress 内存地址
		* @param[in] memorySize 内存大小
		* @return bool
		*/
		bool requestDownload(uint8_t dataFormatIdentifier, uint8_t addressAndLengthFormatIdentifier, uint32_t memoryAddress, size_t memorySize);

		/*
		* @brief 传输数据
		* @param[in] blockSequenceCounter 块序列计数器,需要注意是从1开始
		* @param[in] data 数据
		* @param[in] size 大小
		* @return bool
		*/
		bool transferData(uint8_t blockSequenceCounter, const uint8_t* data, size_t size);

		/*
		* @brief 输入输出控制通过标识
		* @param[in] dataIdentifierHighBit 数据标识高位
		* @param[in] dataIdentifierLowBit 数据标识低位
		* @param[in] ioControlParameter io控制参数
		* @param[in] controlEnableMaskRecord 控制启用掩码记录
		* @param[in] controlEnableMaskRecordSize 控制启用掩码记录大小
		* @return bool
		*/
		bool inputOutputControlByIdentifier(uint8_t dataIdentifierHighBit, uint8_t dataIdentifierLowBit, uint8_t ioControlParameter, uint8_t* controlEnableMaskRecord, size_t controlEnableMaskRecordSize);

		/*
		* @brief 请求传输退出
		* @return bool
		*/
		bool requestTransferExit();

		/*
		* @brief 开始测试仪在线
		* @param[in] period 发送周期
		* @return void
		*/
		void startTesterPresent(int period);

		/*
		* @brief 停止测试仪在线
		* @return void
		*/
		void stopTesterPresent();

		/*
		* @brief 设置发送超时时间
		* @param[in] timeout 超时时间
		* @return void
		*/
		void setSendTimeout(int timeout);

		/*
		* @brief 设置接收超时时间
		* @param[in] timeout 超时时间
		* @return void
		*/
		void setRecvTimeout(int timeout);

	protected:
		/*
		* @brief 发送
		* @param[in] data 数据
		* @param[in] size 大小
		* @return bool
		*/
		bool send(const uint8_t* data, size_t size);

		/*
		* @brief 发送单帧
		* @param[in] data 数据
		* @param[in] size 大小
		* @return bool
		*/
		bool sendSingleFrame(const uint8_t* data, size_t size);

		/*
		* @brief 发送首帧
		* @param[in] data 数据
		* @param[in] size 大小
		* @param[out] sent 已发送大小
		* @return bool
		*/
		bool sendFirstFrame(const uint8_t* data, size_t size, size_t* sent);

		/*
		* @brief 发送连续帧
		* @param[in] sn 连续帧序号
		* @param[in] data 数据
		* @param[in] size 大小
		* @param[out] sent 已发送大小
		* @return bool
		*/
		bool sendConsecutiveFrame(size_t sn, const uint8_t* data, size_t size, size_t* sent);

		/*
		* @brief 发送流控帧
		* @return bool
		*/
		bool sendFlowControl();

		/*
		* @brief 接收
		* @param[out] data 接收的数据
		* @param[in|out] size 接收大小
		* @return bool
		*/
		bool recv(uint8_t* data, size_t* size);

		/*
		* @brief 接收流控帧
		* @param[out] fcStatus 流控状态
		* @param[out] blockSize 接收方一次能接收的数据最大数量
		* @param[out] stMin 最小连续帧间隔
		* @return bool
		*/
		bool recvFlowControl(int* fcStatus, int* blockSize, int* stMin);

		/*
		* @brief 设置最终错误
		* @param[in] fmt 格式化字符串
		* @param[in] ... 可变参数
		* @return void
		*/
		void setLastError(const char* fmt, ...);

		/*
		* @brief 获取帧数据长度代码
		* @param[in] type 类型
		* @param[in] length 实际长度
		* @return 数据长度代码
		*/
		size_t getFrameDlc(FrameType type, size_t length) const;

	private:
		/*
		* @brief 插入错误代码
		* @param[in] id0 标识0
		* @param[in] id1 标识1
		* @param[in] id2 标识2
		* @return void
		*/
		void insertErrorCode(uint8_t id0, uint8_t id1, uint8_t id2);

		/*
		* @brief 获取密钥
		* @param[in] level 解锁级别
		* @param[in] seedData 种子数据
		* @param[in] seedSize 种子大小
		* @param[out] keyData 密钥数据
		* @param[out] keySize 密钥大小
		* @return bool
		* @note 密钥大小,针对不同算法,大小可能有所变化,根据所需修改
		*/
		virtual bool getKey(int level, const uint8_t* seedData, int seedSize, uint8_t* keyData, int* keySize = nullptr);

		/*
		* @brief 获取请求ID
		* @return int
		*/
		int getRequestId() const;

		/*
		* @brief 获取响应ID
		* @return int
		*/
		int getResponseId() const;

		/*
		* @brief 拷贝到缓冲区
		* @param[in] flag 标识
		* @param[in] data 数据
		* @param[in] size 大小
		* @return void
		*/
		void copyToBuffer(int flag, const uint8_t* data, size_t size);

		/*
		* @brief 待机握手线程
		* @param args 参数
		* @return void
		*/
		void onTesterPresent();

	protected:
		uds::CommunicationId m_cid = { 0 };

	private:
		//最终错误
		std::string m_lastError = "未知错误";

		//CAN
		std::shared_ptr<can::Base> m_can;

		//CAN协议类型
		can::ProtoType m_proto;

		//线程退出
		bool m_quit = false;

		//测试仪在线线程运行
		bool m_testerPresentStart = false;

		//测试仪在线发送周期
		int m_testerPresentPeriod = 1000;

		//期望
		uds::Future m_future;

		//can卡通道
		int m_channel = 0;

		//数据长度代码
		int m_dlc = 8;

		//发送超时时间
		int m_sendTimeout = 5000;

		//接收超时时间
		int m_recvTimeout = 5000;

		//帧长度
		//uds::FrameLength m_frameLen;

		//缓冲区
		uds::Buffer m_buffer;

		//下载大小
		size_t m_maxNumberOfBlockLength = 0;

		//诊断会话类型
		uint8_t m_diagnosticSessionType = DiagnosticSessionType::DEFAULT;

		//获取key回调
		GetKeyCallback m_getKeyCb = nullptr;
	};
}
