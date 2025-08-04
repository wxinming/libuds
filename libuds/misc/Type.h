#pragma once
#include <future>
#include "Timer.h"
#include "Export.h"
#define UDS_BUFFER_SIZE 4096

namespace uds {
	/*
	* @brief ��������
	* AVM ȫ������ϵͳ
	* DVR ������Ƶ�ն�
	* LDW ����ƫ��Ԥ��
	* AICS �����������
	* TAP ͸��A��
	* RADAR �״�
	* BSD ä������״�
	* IBP ����B��
	* @note �������� VT_(����)_(������)_(����)
	*/
	enum VehicleType {
		VT_SGMW_201S_AVM,
	};

	/*
	* @brief ��ϻỰ����
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
	* @brief ��ȫ��������
	* @note �˲������ݳ����������,������������ο�
	*/
	enum SecurityAccessType {
		SAT_DEFAULT_LEVEL1 = 0x01,
		SAT_DEFAULT_LEVEL2 = 0x03,
		SAT_DEFAULT_LEVEL3 = 0x05,
		SAT_DEFAULT_LEVEL4 = 0x07,
		//0x07~0x41 Defined By The Vehicle Manufacture
	};

	/*
	* @brief ����ecu����
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
	* @brief ͨѶ��������
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
	* @brief ��ʱ������������
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
	* @brief ��Ϲ�������������
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
	* @brief ��Ϲ����뱨������
	* @note Ŀǰ��֧��0x01, 0x02, 0x0f, 0x11, 0x12, 0x13
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
	* @brief ���̿�������
	*/
	enum RoutineControlType {
		//RCT_ISO_SAE_RESERVED,
		RCT_START_ROUTINE = 0x01,
		RCT_STOP_ROUTINE,
		RCT_REQUEST_ROUTINE_RESULTS,
		//0x04~0x7f ISO SAE RESERVED
	};
	/*
	* @brief ֡����
	*/
	enum FrameType {
		//Single Frame(��֡)
		FT_SF = 0,

		//First Frame(��֡)
		FT_FF = 1,

		//Consecutive Frame(����֡)
		FT_CF = 2,

		//Flow Control Frame(����֡)
		FT_FC = 3,
	};

	/*
	* @brief ����״̬
	*/
	enum FlowStatus {
		//��������
		FS_CONTINUE_TO_SEND,

		//�ȴ�
		FS_WAITTING,

		//���
		FS_OVERFLOW,

		//����0x3~0xf
		//FS_RESERVED
	};

	/*
	* @brief ͨѶID
	*/
	struct UDS_DLL_EXPORT CommunicationId {
		//����id
		struct {
			//����id(����Ѱַ,һ��һ)
			int physical;

			//����id(����Ѱַ,һ�Զ�)
			int function;

			//ģʽ
			int mode;
		} request;

		//��Ӧid
		int response;
	};

	/*
	* @brief ͨѶ״̬
	*/
	enum CommunicationStatus {
		//�ȴ���ʼ
		CS_WAITTING_START = 0,

		//���͵�֡
		CS_SEND_SF = 1,

		//������֡
		CS_SEND_FF = 2,

		//��������֡
		CS_SEND_CF = 3,

		//��������֡
		CS_SEND_FC = 4,

		//���յ�֡
		CS_RECV_SF = 5,

		//������֡
		CS_RECV_FF = 6,

		//��������֡
		CS_RECV_CF = 7,

		//��������֡
		CS_RECV_FC = 8,

		//���
		CS_FINISH = 9,
	};

	/*
	* @brief ����ģʽ
	*/
	enum RequestMode {
		//��������
		RM_PHYSICAL = 0,

		//��������
		RM_FUNCTIONAL = 1,
	};

	/*
	* @brief ����
	*/
	struct UDS_DLL_EXPORT Future {
		std::future<void> tester;
	};

	/*
	* @brief ֡����
	*/
	struct UDS_DLL_EXPORT FrameLength {
		//��֡
		int singleFrame;

		//��֡
		int firstFrame;

		//����֡
		int flowControlFrame;

		//����֡
		int consecutiveFrame;
	};

	/*
	* @brief ����֡
	*/
	struct UDS_DLL_EXPORT FlowControlFrame {
		int status;
		int blockSize;
		//Separation Time minimum
		int stMin;
	};

	/*
	* @brief ������
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

