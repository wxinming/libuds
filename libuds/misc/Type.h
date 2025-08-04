#pragma once
#include <future>
#include "Timer.h"
#include "Export.h"
#define UDS_BUFFER_SIZE 4096

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
	* @note 命名规则 VT_(车企)_(机种名)_(类型)
	*/
	enum VehicleType {
		VT_SGMW_201S_AVM,
	};

	/*
	* @brief 诊断会话类型
	*/
	enum DiagnosticSessionType {
		//DST_ISO_SAE_RESERVED,
		DST_DEFAULT = 0x01,
		DST_PROGRAMMING,
		DST_EXTEND,
		DST_SAFETY_SYSTEM,
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
		SAT_DEFAULT_LEVEL1 = 0x01,
		SAT_DEFAULT_LEVEL2 = 0x03,
		SAT_DEFAULT_LEVEL3 = 0x05,
		SAT_DEFAULT_LEVEL4 = 0x07,
		//0x07~0x41 Defined By The Vehicle Manufacture
	};

	/*
	* @brief 重置ecu类型
	*/
	enum RsetEcuType {
		//RET_ISO_SAE_RESERVED,
		RET_HARD = 0x01,
		RET_KEY_OFF_ON,
		RET_SOFT,
		RET_ENABLE_RAPID_POWER_SHUTDOWN,
		RET_DISABLE_RAPID_POWER_SHUTDOWN,
		//0x06~0x3f ISO SAE Reserved
		//0x40~0x5f Vehicle Manufacturer Specific
		//0x60~0x7e System Supplier Specific
		//0x7f ISO SAE Reserved
	};

	/*
	* @brief 通讯控制类型
	*/
	enum CommunicationControlType {
		CCT_ENABLE_RX_AND_TX,
		CCT_ENABLE_RX_AND_DISABLE_TX,
		CCT_DISABLE_RX_AND_ENABLE_TX,
		CCT_DISABLE_RX_AND_TX,
		CCT_ENABLE_RX_AND_DISABLE_TX_WITH_ENHANCED_ADDRESS_INFORMATION,
		CCT_ENABLE_RX_AND_TX_WITH_ENHANCED_ADDRESS_INFORMATION,
		//0x06~0x3f ISO SAE Reserved
		//0x40~0x5f Vehicle Manufacturer Specific
		//0x60~0x7e System Supplier Specific
		//0x7f ISO SAE Reserved
	};

	/*
	* @brief 定时参数访问类型
	*/
	enum TimingParameterAccessType {
		//TPAT_ISO_SAE_RESERVED,
		TPAP_READ_EXTENED_TIMING_PARAMETER_SET = 0x01,
		TPAP_SET_TIMING_PARAMETERS_TO_DEFAULT_VALUES,
		TPAP_READ_CURRENTLY_ACTIVE_TIMING_PARAMETERS,
		TPAP_SET_TIMING_PARAMETERS_TO_GIVEN_VALUES,
		//0x05~0xff ISO SAE Reserved
	};

	/*
	* @brief 诊断故障码设置类型
	*/
	enum DtcSettingType {
		//DST_ISO_SAE_RESERVED,
		DST_ON = 0x01,
		DST_OFF,
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
		//DRT_ISO_SAE_RESERVED,
		DRT_REPORT_NUMBER_OF_DTC_BY_STATUS_MASK = 0x01,
		DRT_REPORT_DTC_BY_STATUS_MASK,
		DRT_REPORT_DTC_SNAPSHOT_IDENTIFICATION,
		DRT_REPORT_DTC_SNAPSHOT_RECORD_BY_DTC_NUMBER,
		DRT_REPORT_DTC_STORED_DATA_BY_RECORD_NUMBER,
		DRT_REPORT_DTC_EXT_DATA_RECORD_BY_DTC_NUMBER,
		DRT_REPORT_NUMBER_OF_DTC_BY_SEVERITY_MASK_RECORD,
		DRT_REPORT_DTC_BY_SEVERITY_MASK_RECORD,
		DRT_REPORT_SEVERITY_INFORMATION_OF_DTC,
		DRT_REPORT_SUPPORTED_DTC,
		DRT_REPORT_FIRST_TEST_FAILED_DTC,
		DRT_REPORT_FIRST_CONFIRMED_DTC,
		DRT_REPORT_MOST_RECENT_TEST_FAILED_DTC,
		DRT_REPORT_MOST_RECENT_CONFIRMED_DTC,
		DRT_REPORT_MIRROR_MEMORY_DTC_BY_STATUS_MASK,
		DRT_REPORT_MIRROR_MEMORY_DTC_EXT_DATA_RECORD_BY_DTC_NUMBER,
		DRT_REPORT_NUMBER_OF_MIRROR_MEMORY_DTC_BY_STATUS_MASK,
		DRT_REPORT_NUMBER_OF_EMISSIONS_OBD_DTC_BY_STATUS_MASK,
		DRT_REPORT_EMISSIONS_OBD_DTC_BY_STATUS_MASK,
		DRT_REPORT_DTC_FAULT_DETECTION_COUNTER,
		DRT_REPORT_DTC_WITH_PERMANENT_STATUS,
		DRT_REPORT_DTC_EXT_DATA_RECORD_BY_RECORD_NUMBER,
		DRT_REPORT_USER_DEF_MEMORY_DTC_BY_STATUS_MASK,
		DRT_REPORT_USER_DEF_MEMORY_DTC_SNAPSHOT_RECORD_BY_DTC_NUMBER,
		DRT_REPORT_USER_DEF_MEMORY_DTC_EXT_DATA_RECORD_BY_DTC_NUMBER,
		//0X1A~0X41 ISO SAE RESERVED
		DRT_REPORT_WWHOBD_DTC_BY_MASK_RECORD = 0X42,
		//0X43~0X54 ISO SAE RESERVED
		DRT_REPORT_WWHOBD_DTC_WITH_PERMANENT_STATUS = 0x55,
		//0x56~0x7F ISO SAE Reserved
	};

	/*
	* @brief 例程控制类型
	*/
	enum RoutineControlType {
		//RCT_ISO_SAE_RESERVED,
		RCT_START_ROUTINE = 0x01,
		RCT_STOP_ROUTINE,
		RCT_REQUEST_ROUTINE_RESULTS,
		//0x04~0x7f ISO SAE RESERVED
	};
	/*
	* @brief 帧类型
	*/
	enum FrameType {
		//Single Frame(单帧)
		FT_SF = 0,

		//First Frame(首帧)
		FT_FF = 1,

		//Consecutive Frame(连续帧)
		FT_CF = 2,

		//Flow Control Frame(流控帧)
		FT_FC = 3,
	};

	/*
	* @brief 流控状态
	*/
	enum FlowStatus {
		//继续发送
		FS_CONTINUE_TO_SEND,

		//等待
		FS_WAITTING,

		//溢出
		FS_OVERFLOW,

		//保留0x3~0xf
		//FS_RESERVED
	};

	/*
	* @brief 通讯ID
	*/
	struct UDS_DLL_EXPORT CommunicationId {
		//请求id
		struct {
			//物理id(物理寻址,一对一)
			int physical;

			//功能id(功能寻址,一对多)
			int function;

			//模式
			int mode;
		} request;

		//响应id
		int response;
	};

	/*
	* @brief 通讯状态
	*/
	enum CommunicationStatus {
		//等待开始
		CS_WAITTING_START = 0,

		//发送单帧
		CS_SEND_SF = 1,

		//发送首帧
		CS_SEND_FF = 2,

		//发送连续帧
		CS_SEND_CF = 3,

		//发送流控帧
		CS_SEND_FC = 4,

		//接收单帧
		CS_RECV_SF = 5,

		//接收首帧
		CS_RECV_FF = 6,

		//接收连续帧
		CS_RECV_CF = 7,

		//接收流控帧
		CS_RECV_FC = 8,

		//完成
		CS_FINISH = 9,
	};

	/*
	* @brief 请求模式
	*/
	enum RequestMode {
		//物理请求
		RM_PHYSICAL = 0,

		//功能请求
		RM_FUNCTIONAL = 1,
	};

	/*
	* @brief 期望
	*/
	struct UDS_DLL_EXPORT Future {
		std::future<void> tester;
	};

	/*
	* @brief 帧长度
	*/
	struct UDS_DLL_EXPORT FrameLength {
		//单帧
		int singleFrame;

		//首帧
		int firstFrame;

		//流控帧
		int flowControlFrame;

		//连续帧
		int consecutiveFrame;
	};

	/*
	* @brief 流控帧
	*/
	struct UDS_DLL_EXPORT FlowControlFrame {
		int status;
		int blockSize;
		//Separation Time minimum
		int stMin;
	};

	/*
	* @brief 缓冲区
	*/
	struct UDS_DLL_EXPORT Buffer {
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

