#pragma once
#include "misc/Base.h"

namespace uds {
	/*
	* @brief �Զ��ͷ�new
	* @param[in] can can����
	* @param[in] channel ʹ��can���ڼ���ͨ������udsͨѶ
	* @param[in] proto Э������
	* @param[in] dlc ���ݳ��ȴ���[CAN 8bytes],[CANFD 12,16,20,24,32,48,64bytes]
	* @param[in] cid ͨѶid
	* @param[in] callback ��ȡkey�ص�����Ϊnullptr
	* @return std::shared_ptr<uds::Base>
	*/
	UDS_DLL_EXPORT std::shared_ptr<uds::Base> autoReleaseNew(std::shared_ptr<can::Base> can, int channel = 0, 
		int proto = can::PT_CAN, int dlc = 8, CommunicationId cid = { 0 }, GetKeyCallback callback = nullptr);

	/*
	* @brief �Զ��ͷ�new
	* @param[in] vehicle ��������
	* @param[in] can can����
	* @param[in] channel ʹ��can���ڼ���ͨ������udsͨѶ
	* @param[in] proto Э������
	* @param[in] dlc ���ݳ��ȴ���[CAN 8bytes],[CANFD 12,16,20,24,32,48,64bytes]
	* @return std::shared_ptr<uds::Base>
	*/
	UDS_DLL_EXPORT std::shared_ptr<uds::Base> autoReleaseNew(uds::VehicleType vehicle,
		std::shared_ptr<can::Base> can, int channel = 0, int proto = can::PT_CAN, int dlc = 8);
}

