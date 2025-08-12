#pragma once
#include <future>
#include "Timer.h"
#include "Export.h"
#define UDS_BUFFER_SIZE 4096
#define UDS_CAN_MAX_SINGLE_FRAME_LENGTH 7
#define UDS_CANFD_MAX_SINGLE_FRAME_LENGTH 62

namespace uds {
	/*
	* @brief 车辆类型
	* AVM 全景环视系统
	* DVR 车载视频终端
	* LDW 车道偏离预警
	* AICS 智能网关域控
	* TAP 透明A柱
	* RADAR 雷达
	* BSD 盲区检测雷达
	* IBP 智能B柱
	* @note 命名规则 (车企)_(机种名)_(类型)
	*/
	enum VehicleType {
		SGMW_201S_AVM,
	};

	/*
	* @brief 诊断会话类型
	*/
	enum DiagnosticSessionType {
		//ISO_SAE_RESERVED,
		DEFAULT = 0x01,
		PROGRAMMING,
		EXTEND,
		SAFETY_SYSTEM,
		//0x05~0x3f ISO SAE Reserved
		//0x40~0x5f Vehicle Manufacturer Specific
		//0x60~0x7e System Supplier Specific
		//0x7f ISO SAE Reserved
	};

	/*
	* @brief 安全访问类型
	* @note 此参数根据车厂定义而定,下面参数仅供参考
	*/
	enum SecurityAccessType {
		DEFAULT_LEVEL1 = 0x01,
		DEFAULT_LEVEL2 = 0x03,
		DEFAULT_LEVEL3 = 0x05,
		DEFAULT_LEVEL4 = 0x07,
		//0x07~0x41 Defined By The Vehicle Manufacture
	};

	/*
	* @brief 重置ecu类型
	*/
	enum RsetEcuType {
		//ISO_SAE_RESERVED,
		HARD = 0x01,
		KEY_OFF_ON,
		SOFT,
		ENABLE_RAPID_POWER_SHUTDOWN,
		DISABLE_RAPID_POWER_SHUTDOWN,
		//0x06~0x3f ISO SAE Reserved
		//0x40~0x5f Vehicle Manufacturer Specific
		//0x60~0x7e System Supplier Specific
		//0x7f ISO SAE Reserved
	};

	/*
	* @brief 通讯控制类型
	*/
	enum CommunicationControlType {
		ENABLE_RX_AND_TX,
		ENABLE_RX_AND_DISABLE_TX,
		DISABLE_RX_AND_ENABLE_TX,
		DISABLE_RX_AND_TX,
		ENABLE_RX_AND_DISABLE_TX_WITH_ENHANCED_ADDRESS_INFORMATION,
		ENABLE_RX_AND_TX_WITH_ENHANCED_ADDRESS_INFORMATION,
		//0x06~0x3f ISO SAE Reserved
		//0x40~0x5f Vehicle Manufacturer Specific
		//0x60~0x7e System Supplier Specific
		//0x7f ISO SAE Reserved
	};

	/*
	* @brief 定时参数访问类型
	*/
	enum TimingParameterAccessType {
		//ISO_SAE_RESERVED,
		READ_EXTENED_TIMING_PARAMETER_SET = 0x01,
		SET_TIMING_PARAMETERS_TO_DEFAULT_VALUES,
		READ_CURRENTLY_ACTIVE_TIMING_PARAMETERS,
		SET_TIMING_PARAMETERS_TO_GIVEN_VALUES,
		//0x05~0xff ISO SAE Reserved
	};

	/*
	* @brief 诊断故障码设置类型
	*/
	enum DtcSettingType {
		//ISO_SAE_RESERVED,
		ON = 0x01,
		OFF,
		//0x03~0x3f ISO SAE Reserved
		//0x40~0x5f Vehicle Manufacturer Specific
		//0x60~0x7e System Supplier Specific
		//0x7f ISO SAE Reserved
	};

	/*
	* @brief 诊断故障码报告类型
	* @note 目前仅支持0x01, 0x02, 0x0f, 0x11, 0x12, 0x13
	*/
	enum DtcReportType {
		//ISO_SAE_RESERVED,
		REPORT_NUMBER_OF_DTC_BY_STATUS_MASK = 0x01,
		REPORT_DTC_BY_STATUS_MASK,
		REPORT_DTC_SNAPSHOT_IDENTIFICATION,
		REPORT_DTC_SNAPSHOT_RECORD_BY_DTC_NUMBER,
		REPORT_DTC_STORED_DATA_BY_RECORD_NUMBER,
		REPORT_DTC_EXT_DATA_RECORD_BY_DTC_NUMBER,
		REPORT_NUMBER_OF_DTC_BY_SEVERITY_MASK_RECORD,
		REPORT_DTC_BY_SEVERITY_MASK_RECORD,
		REPORT_SEVERITY_INFORMATION_OF_DTC,
		REPORT_SUPPORTED_DTC,
		REPORT_FIRST_TEST_FAILED_DTC,
		REPORT_FIRST_CONFIRMED_DTC,
		REPORT_MOST_RECENT_TEST_FAILED_DTC,
		REPORT_MOST_RECENT_CONFIRMED_DTC,
		REPORT_MIRROR_MEMORY_DTC_BY_STATUS_MASK,
		REPORT_MIRROR_MEMORY_DTC_EXT_DATA_RECORD_BY_DTC_NUMBER,
		REPORT_NUMBER_OF_MIRROR_MEMORY_DTC_BY_STATUS_MASK,
		REPORT_NUMBER_OF_EMISSIONS_OBD_DTC_BY_STATUS_MASK,
		REPORT_EMISSIONS_OBD_DTC_BY_STATUS_MASK,
		REPORT_DTC_FAULT_DETECTION_COUNTER,
		REPORT_DTC_WITH_PERMANENT_STATUS,
		REPORT_DTC_EXT_DATA_RECORD_BY_RECORD_NUMBER,
		REPORT_USER_DEF_MEMORY_DTC_BY_STATUS_MASK,
		REPORT_USER_DEF_MEMORY_DTC_SNAPSHOT_RECORD_BY_DTC_NUMBER,
		REPORT_USER_DEF_MEMORY_DTC_EXT_DATA_RECORD_BY_DTC_NUMBER,
		//0X1A~0X41 ISO SAE RESERVED
		REPORT_WWHOBD_DTC_BY_MASK_RECORD = 0X42,
		//0X43~0X54 ISO SAE RESERVED
		REPORT_WWHOBD_DTC_WITH_PERMANENT_STATUS = 0x55,
		//0x56~0x7F ISO SAE Reserved
	};

	/*
	* @brief 例程控制类型
	*/
	enum RoutineControlType {
		//ISO_SAE_RESERVED,
		START_ROUTINE = 0x01,
		STOP_ROUTINE,
		REQUEST_ROUTINE_RESULTS,
		//0x04~0x7f ISO SAE RESERVED
	};
	/*
	* @brief 帧类型
	*/
	enum FrameType {
		//Single Frame(单帧)
		SINGLE_FRAME = 0,

		//First Frame(首帧)
		FIRST_FRAME = 1,

		//Consecutive Frame(连续帧)
		CONSECUTIVE_FRAME = 2,

		//Flow Control Frame(流控帧)
		FLOW_CONTROL = 3,
	};

	/*
	* @brief 流控状态
	*/
	enum FlowStatus {
		//继续发送
		CONTINUE_TO_SEND,

		//等待
		WAITTING,

		//溢出
		OVERFLOWS,

		//保留0x3~0xf
		//RESERVED
	};

	/*
	* @brief 请求模式
	*/
	enum RequestMode {
		//物理请求
		PHYSICAL = 0,

		//功能请求
		FUNCTIONAL = 1,
	};

	/*
	* @brief 通讯ID
	*/
	struct LIBUDS_DLL_EXPORT CommunicationId {
		//请求id
		struct Request {
			//物理id(物理寻址,一对一)
			int physical;

			//功能id(功能寻址,一对多)
			int function;

			//模式
			RequestMode mode;
		} request;

		//响应id
		int response;
	};

	/*
	* @brief 通讯状态
	*/
	enum CommunicationStatus {
		//等待开始
		WAITTING_START = 0,

		//发送单帧
		SEND_SINGLE_FRAME = 1,

		//发送首帧
		SEND_FIRST_FRAME = 2,

		//发送连续帧
		SEND_CONSECUTIVE_FRAME = 3,

		//发送流控帧
		SEND_FLOW_CONTROL = 4,

		//接收单帧
		RECV_SINGLE_FRAME = 5,

		//接收首帧
		RECV_FIRST_FRAME = 6,

		//接收连续帧
		RECV_CONSECUTIVE_FRAME = 7,

		//接收流控帧
		RECV_FLOW_CONTROL = 8,

		//完成
		FINISH = 9,
	};

	/*
	* @brief 期望
	*/
	struct LIBUDS_DLL_EXPORT Future {
		std::future<void> tester;
	};

	/*
	* @brief 帧长度
	*/
	/*
	struct LIBUDS_DLL_EXPORT FrameLength {
		//单帧
		int singleFrame;

		//首帧
		int firstFrame;

		//流控帧
		int flowControlFrame;

		//连续帧
		int consecutiveFrame;
	};
	*/

	/*
	* @brief 流控帧
	*/
	struct LIBUDS_DLL_EXPORT FlowControlFrame {
		int status;
		int blockSize;
		//Separation Time minimum
		int stMin;
	};

	/*
	* @brief 缓冲区
	*/
	struct LIBUDS_DLL_EXPORT Buffer {
		Buffer(size_t size = UDS_BUFFER_SIZE);
		~Buffer();
		Buffer(const Buffer& buffer);
		Buffer& operator=(const Buffer& buffer);
		Buffer(Buffer&& buffer) noexcept;
		Buffer& operator=(Buffer&& buffer) noexcept;

		struct {
			uint8_t* data;
			size_t size;
		} send;

		struct {
			uint8_t* data;
			size_t size;
		} recv;
	};

	typedef std::function<bool(int, const uint8_t*, int, uint8_t*, int*)> GetKeyCallback;
}

