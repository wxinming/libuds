#pragma once
#include <future>
#include "Timer.h"
#include "Export.h"
#define UDS_BUFFER_SIZE 4096
#define UDS_CAN_MAX_SINGLE_FRAME_LENGTH 7
#define UDS_CANFD_MAX_SINGLE_FRAME_LENGTH 62

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
	* @note �������� (����)_(������)_(����)
	*/
	enum VehicleType {
		SGMW_201S_AVM,
	};

	/*
	* @brief ��ϻỰ����
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
	* @brief ��ȫ��������
	* @note �˲������ݳ����������,������������ο�
	*/
	enum SecurityAccessType {
		DEFAULT_LEVEL1 = 0x01,
		DEFAULT_LEVEL2 = 0x03,
		DEFAULT_LEVEL3 = 0x05,
		DEFAULT_LEVEL4 = 0x07,
		//0x07~0x41 Defined By The Vehicle Manufacture
	};

	/*
	* @brief ����ecu����
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
	* @brief ͨѶ��������
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
	* @brief ��ʱ������������
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
	* @brief ��Ϲ�������������
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
	* @brief ��Ϲ����뱨������
	* @note Ŀǰ��֧��0x01, 0x02, 0x0f, 0x11, 0x12, 0x13
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
	* @brief ���̿�������
	*/
	enum RoutineControlType {
		//ISO_SAE_RESERVED,
		START_ROUTINE = 0x01,
		STOP_ROUTINE,
		REQUEST_ROUTINE_RESULTS,
		//0x04~0x7f ISO SAE RESERVED
	};
	/*
	* @brief ֡����
	*/
	enum FrameType {
		//Single Frame(��֡)
		SINGLE_FRAME = 0,

		//First Frame(��֡)
		FIRST_FRAME = 1,

		//Consecutive Frame(����֡)
		CONSECUTIVE_FRAME = 2,

		//Flow Control Frame(����֡)
		FLOW_CONTROL = 3,
	};

	/*
	* @brief ����״̬
	*/
	enum FlowStatus {
		//��������
		CONTINUE_TO_SEND,

		//�ȴ�
		WAITTING,

		//���
		OVERFLOWS,

		//����0x3~0xf
		//RESERVED
	};

	/*
	* @brief ����ģʽ
	*/
	enum RequestMode {
		//��������
		PHYSICAL = 0,

		//��������
		FUNCTIONAL = 1,
	};

	/*
	* @brief ͨѶID
	*/
	struct LIBUDS_DLL_EXPORT CommunicationId {
		//����id
		struct Request {
			//����id(����Ѱַ,һ��һ)
			int physical;

			//����id(����Ѱַ,һ�Զ�)
			int function;

			//ģʽ
			RequestMode mode;
		} request;

		//��Ӧid
		int response;
	};

	/*
	* @brief ͨѶ״̬
	*/
	enum CommunicationStatus {
		//�ȴ���ʼ
		WAITTING_START = 0,

		//���͵�֡
		SEND_SINGLE_FRAME = 1,

		//������֡
		SEND_FIRST_FRAME = 2,

		//��������֡
		SEND_CONSECUTIVE_FRAME = 3,

		//��������֡
		SEND_FLOW_CONTROL = 4,

		//���յ�֡
		RECV_SINGLE_FRAME = 5,

		//������֡
		RECV_FIRST_FRAME = 6,

		//��������֡
		RECV_CONSECUTIVE_FRAME = 7,

		//��������֡
		RECV_FLOW_CONTROL = 8,

		//���
		FINISH = 9,
	};

	/*
	* @brief ����
	*/
	struct LIBUDS_DLL_EXPORT Future {
		std::future<void> tester;
	};

	/*
	* @brief ֡����
	*/
	/*
	struct LIBUDS_DLL_EXPORT FrameLength {
		//��֡
		int singleFrame;

		//��֡
		int firstFrame;

		//����֡
		int flowControlFrame;

		//����֡
		int consecutiveFrame;
	};
	*/

	/*
	* @brief ����֡
	*/
	struct LIBUDS_DLL_EXPORT FlowControlFrame {
		int status;
		int blockSize;
		//Separation Time minimum
		int stMin;
	};

	/*
	* @brief ������
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

