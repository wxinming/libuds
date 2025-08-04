#pragma once
#include "misc/Base.h"

namespace uds {
	/*
	* @brief 自动释放new
	* @param[in] can can卡类
	* @param[in] channel 使用can卡第几个通道进行uds通讯
	* @param[in] proto 协议类型
	* @param[in] dlc 数据长度代码[CAN 8bytes],[CANFD 12,16,20,24,32,48,64bytes]
	* @param[in] cid 通讯id
	* @param[in] callback 获取key回调可以为nullptr
	* @return std::shared_ptr<uds::Base>
	*/
	UDS_DLL_EXPORT std::shared_ptr<uds::Base> autoReleaseNew(std::shared_ptr<can::Base> can, int channel = 0, 
		int proto = can::PT_CAN, int dlc = 8, CommunicationId cid = { 0 }, GetKeyCallback callback = nullptr);

	/*
	* @brief 自动释放new
	* @param[in] vehicle 车辆类型
	* @param[in] can can卡类
	* @param[in] channel 使用can卡第几个通道进行uds通讯
	* @param[in] proto 协议类型
	* @param[in] dlc 数据长度代码[CAN 8bytes],[CANFD 12,16,20,24,32,48,64bytes]
	* @return std::shared_ptr<uds::Base>
	*/
	UDS_DLL_EXPORT std::shared_ptr<uds::Base> autoReleaseNew(uds::VehicleType vehicle,
		std::shared_ptr<can::Base> can, int channel = 0, int proto = can::PT_CAN, int dlc = 8);
}

