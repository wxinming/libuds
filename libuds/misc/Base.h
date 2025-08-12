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
		* @brief ����
		* @param[in] base can������
		* @param[in] channel can��ͨ��
		* @param[in] proto Э������
		* @param[in] cid ͨѶid
		* @param[in] callback ��ȡkey�ص�����Ϊnullptr
		*/
		Base(std::shared_ptr<can::Base> base, int channel = 0, can::ProtoType proto = can::ProtoType::CAN, CommunicationId cid = { 0 }, GetKeyCallback callback = nullptr);

		/*
		* @brief ����
		*/
		virtual ~Base();

		/*
		* @brief ��������ɾ��
		*/
		Base(const uds::Base&) = delete;

		/*
		* @brief ��ֵ����ɾ��
		*/
		Base& operator=(const uds::Base&) = delete;

		/*
		* @brief ��ȡ���մ���
		* @return std::string
		*/
		std::string getLastError() const;

		/*
		* @brief ����ͨѶID
		* @param[in] id id
		* @return void
		*/
		void setCommunicationId(const uds::CommunicationId& id);

		/*
		* @brief ��ȡͨѶID
		* @return uds::CommunicationId
		*/
		uds::CommunicationId getCommunicationId() const;

		/*
		* @brief ���û�ȡ��Կ�ص�
		* @param[in] callback �ص�
		* @return void
		*/
		void setGetKeyCallback(uds::GetKeyCallback callback);

		/*
		* @brief ��ȡͨѶ������
		* @return uds::Buffer
		*/
		uds::Buffer getCommunicationBuffer() const;

		/*
		* @brief ��ϻỰ����
		* @param[in] diagnosticSessionType ��ϻỰ����
		* @return bool
		*/
		bool diagnosticSessionControl(uint8_t diagnosticSessionType);

		/*
		* @brief ����������
		* @param[in] resetType ��������
		* @return bool
		*/
		bool ecuReset(uint8_t resetType);

		/*
		* @brief ��ȫ����
		* @param[in] securityAccessType ��ȫ��������
		* @param[in] errorKey �Ƿ�Ϊ������Կ
		* @return bool
		*/
		bool securityAccess(uint8_t securityAccessType, bool errorKey = false);

		/*
		* @brief ͨ�ſ���
		* @param[in] controlType ��������
		* @param[in] communicationType ͨѶ����
		* @param[in] nodeIdentificationNumber �ڵ�ʶ���
		* @return bool
		* @note ֻ�е�communicationTypeΪ4��5ʱ,nodeIdentificationNumber��������
		*/
		bool communicationControl(uint8_t controlType, uint8_t communicationType, uint16_t nodeIdentificationNumber = 0);

		/*
		* @brief ��������
		* @param[in] zeroSubFunction ���ӹ���
		* @return bool
		*/
		bool testerPresent(uint8_t zeroSubFunction);

		/*
		* @brief ���ʶ�ʱ����
		* @param[in] timingParameterAccessType ��ʱ������������
		* @return bool
		*/
		bool accessTimingParameter(uint8_t timingParameterAccessType);

		/*
		* @brief ��ȫ���ݴ���
		* @return bool
		*/
		bool securedDataTransmission();

		/*
		* @brief ����DTC����
		* @param[in] DtcSettingType ��Ϲ�������������
		* @return bool
		*/
		bool controlDtcSetting(uint8_t DtcSettingType);

		/*
		* @brief ��������Ϣ
		* @return bool
		*/
		bool clearDiagnosticInformation();

		/*
		* @brief ��ȡDtc��Ϣ
		* @param[in] reportType ��������
		* @param[in] statusMask ״̬����
		* @param[out] data ��ȡ������
		* @param[in|out] size ��ȡ�Ĵ�С
		* @return bool
		*/
		bool readDtcInformation(uint8_t reportType, uint8_t statusMask, uint8_t* data, size_t* size);

		/*
		* @brief ͨ����ʶ����ȡ����
		* @param[in] dataIdentifierHighBit ���ݱ�ʶ��λ
		* @param[in] dataIdentifierLowBit ���ݱ�ʶ��λ
		* @param[in] data ��ȡ������
		* @param[in|out] size ��ȡ�Ĵ�С
		* @return bool
		*/
		bool readDataByIdentifier(uint8_t dataIdentifierHighBit, uint8_t dataIdentifierLowBit, uint8_t* data, size_t* size);

		/*
		* @brief ͨ����ʶ��д������
		* @param[in] dataIdentifierHighBit ���ݱ�ʶ��λ
		* @param[in] dataIdentifierLowBit ���ݱ�ʶ��λ
		* @param[in] data ��Ҫд�������
		* @param[in] size д������ݴ�С
		* @return bool
		*/
		bool writeDataByIdentifier(uint8_t dataIdentifierHighBit, uint8_t dataIdentifierLowBit, const uint8_t* data, size_t size);

		/*
		* @brief ���̿���
		* @param[in] routineControlType ���̿�������
		* @param[in] routineIdentifierHighBit ���̱�ʶ����λ
		* @param[in] routineIdentifierLowBit ���̱�ʶ����λ
		* @param[in] data ��Ҫ���͵�����
		* @param[in] size �������ݵĴ�С
		* @return bool
		*/
		bool routineControl(uint8_t routineControlType, uint8_t routineIdentifierHighBit, uint8_t routineIdentifierLowBit, const uint8_t* data, size_t size);

		/*
		* @brief ��������
		* @param[in] dataFormatIdentifier ���ݸ�ʽ��ʶ��
		* @param[in] addressAndLengthFormatIdentifier ��ַ�ͳ��ȸ�ʽ��ʶ��
		* @param[in] memoryAddress �ڴ��ַ
		* @param[in] memorySize �ڴ��С
		* @return bool
		*/
		bool requestDownload(uint8_t dataFormatIdentifier, uint8_t addressAndLengthFormatIdentifier, uint32_t memoryAddress, size_t memorySize);

		/*
		* @brief ��������
		* @param[in] blockSequenceCounter �����м�����,��Ҫע���Ǵ�1��ʼ
		* @param[in] data ����
		* @param[in] size ��С
		* @return bool
		*/
		bool transferData(uint8_t blockSequenceCounter, const uint8_t* data, size_t size);

		/*
		* @brief �����������ͨ����ʶ
		* @param[in] dataIdentifierHighBit ���ݱ�ʶ��λ
		* @param[in] dataIdentifierLowBit ���ݱ�ʶ��λ
		* @param[in] ioControlParameter io���Ʋ���
		* @param[in] controlEnableMaskRecord �������������¼
		* @param[in] controlEnableMaskRecordSize �������������¼��С
		* @return bool
		*/
		bool inputOutputControlByIdentifier(uint8_t dataIdentifierHighBit, uint8_t dataIdentifierLowBit, uint8_t ioControlParameter, uint8_t* controlEnableMaskRecord, size_t controlEnableMaskRecordSize);

		/*
		* @brief �������˳�
		* @return bool
		*/
		bool requestTransferExit();

		/*
		* @brief ��ʼ����������
		* @param[in] period ��������
		* @return void
		*/
		void startTesterPresent(int period);

		/*
		* @brief ֹͣ����������
		* @return void
		*/
		void stopTesterPresent();

		/*
		* @brief ���÷��ͳ�ʱʱ��
		* @param[in] timeout ��ʱʱ��
		* @return void
		*/
		void setSendTimeout(int timeout);

		/*
		* @brief ���ý��ճ�ʱʱ��
		* @param[in] timeout ��ʱʱ��
		* @return void
		*/
		void setRecvTimeout(int timeout);

	protected:
		/*
		* @brief ����
		* @param[in] data ����
		* @param[in] size ��С
		* @return bool
		*/
		bool send(const uint8_t* data, size_t size);

		/*
		* @brief ���͵�֡
		* @param[in] data ����
		* @param[in] size ��С
		* @return bool
		*/
		bool sendSingleFrame(const uint8_t* data, size_t size);

		/*
		* @brief ������֡
		* @param[in] data ����
		* @param[in] size ��С
		* @param[out] sent �ѷ��ʹ�С
		* @return bool
		*/
		bool sendFirstFrame(const uint8_t* data, size_t size, size_t* sent);

		/*
		* @brief ��������֡
		* @param[in] sn ����֡���
		* @param[in] data ����
		* @param[in] size ��С
		* @param[out] sent �ѷ��ʹ�С
		* @return bool
		*/
		bool sendConsecutiveFrame(size_t sn, const uint8_t* data, size_t size, size_t* sent);

		/*
		* @brief ��������֡
		* @return bool
		*/
		bool sendFlowControl();

		/*
		* @brief ����
		* @param[out] data ���յ�����
		* @param[in|out] size ���մ�С
		* @return bool
		*/
		bool recv(uint8_t* data, size_t* size);

		/*
		* @brief ��������֡
		* @param[out] fcStatus ����״̬
		* @param[out] blockSize ���շ�һ���ܽ��յ������������
		* @param[out] stMin ��С����֡���
		* @return bool
		*/
		bool recvFlowControl(int* fcStatus, int* blockSize, int* stMin);

		/*
		* @brief �������մ���
		* @param[in] fmt ��ʽ���ַ���
		* @param[in] ... �ɱ����
		* @return void
		*/
		void setLastError(const char* fmt, ...);

		/*
		* @brief ��ȡ֡���ݳ��ȴ���
		* @param[in] type ����
		* @param[in] length ʵ�ʳ���
		* @return ���ݳ��ȴ���
		*/
		size_t getFrameDlc(FrameType type, size_t length) const;

	private:
		/*
		* @brief ����������
		* @param[in] id0 ��ʶ0
		* @param[in] id1 ��ʶ1
		* @param[in] id2 ��ʶ2
		* @return void
		*/
		void insertErrorCode(uint8_t id0, uint8_t id1, uint8_t id2);

		/*
		* @brief ��ȡ��Կ
		* @param[in] level ��������
		* @param[in] seedData ��������
		* @param[in] seedSize ���Ӵ�С
		* @param[out] keyData ��Կ����
		* @param[out] keySize ��Կ��С
		* @return bool
		* @note ��Կ��С,��Բ�ͬ�㷨,��С���������仯,���������޸�
		*/
		virtual bool getKey(int level, const uint8_t* seedData, int seedSize, uint8_t* keyData, int* keySize = nullptr);

		/*
		* @brief ��ȡ����ID
		* @return int
		*/
		int getRequestId() const;

		/*
		* @brief ��ȡ��ӦID
		* @return int
		*/
		int getResponseId() const;

		/*
		* @brief ������������
		* @param[in] flag ��ʶ
		* @param[in] data ����
		* @param[in] size ��С
		* @return void
		*/
		void copyToBuffer(int flag, const uint8_t* data, size_t size);

		/*
		* @brief ���������߳�
		* @param args ����
		* @return void
		*/
		void onTesterPresent();

	protected:
		uds::CommunicationId m_cid = { 0 };

	private:
		//���մ���
		std::string m_lastError = "δ֪����";

		//CAN
		std::shared_ptr<can::Base> m_can;

		//CANЭ������
		can::ProtoType m_proto;

		//�߳��˳�
		bool m_quit = false;

		//�����������߳�����
		bool m_testerPresentStart = false;

		//���������߷�������
		int m_testerPresentPeriod = 1000;

		//����
		uds::Future m_future;

		//can��ͨ��
		int m_channel = 0;

		//���ݳ��ȴ���
		int m_dlc = 8;

		//���ͳ�ʱʱ��
		int m_sendTimeout = 5000;

		//���ճ�ʱʱ��
		int m_recvTimeout = 5000;

		//֡����
		//uds::FrameLength m_frameLen;

		//������
		uds::Buffer m_buffer;

		//���ش�С
		size_t m_maxNumberOfBlockLength = 0;

		//��ϻỰ����
		uint8_t m_diagnosticSessionType = DiagnosticSessionType::DEFAULT;

		//��ȡkey�ص�
		GetKeyCallback m_getKeyCb = nullptr;
	};
}
