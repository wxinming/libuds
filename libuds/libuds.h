#pragma once
#include "misc/Base.h"

namespace uds {
	/*
	* @brief �Զ��ͷ�new
	* @param[in] can can����
	* @param[in] channel ʹ��can���ڼ���ͨ������udsͨѶ
	* @param[in] proto Э������
	* @param[in] cid ͨѶid
	* @param[in] callback ��ȡkey�ص�����Ϊnullptr
	* @return std::shared_ptr<uds::Base>
	*/
	LIBUDS_DLL_EXPORT std::shared_ptr<uds::Base> autoReleaseNew(std::shared_ptr<can::Base> can, int channel = 0, 
		can::ProtoType proto = can::ProtoType::CAN, CommunicationId cid = { 0 }, GetKeyCallback callback = nullptr);

	/*
	* @brief �Զ��ͷ�new
	* @param[in] vehicle ��������
	* @param[in] can can����
	* @param[in] channel ʹ��can���ڼ���ͨ������udsͨѶ
	* @param[in] proto Э������
	* @return std::shared_ptr<uds::Base>
	*/
	LIBUDS_DLL_EXPORT std::shared_ptr<uds::Base> autoReleaseNew(uds::VehicleType vehicle,
		std::shared_ptr<can::Base> can, int channel = 0, can::ProtoType proto = can::ProtoType::CAN);
}

