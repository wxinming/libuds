#include "Vehicle.h"

uds::Sgmw201sAvm::Sgmw201sAvm(std::shared_ptr<can::Base> base, int channel, int proto, int dlc)
	:Base(base, channel, proto, dlc)
{
	m_cid.request.physical = 0x724;
	m_cid.request.function = 0x7df;
	m_cid.request.mode = RM_PHYSICAL;
	m_cid.response = 0x72c;
}

uds::Sgmw201sAvm::~Sgmw201sAvm()
{
}

bool uds::Sgmw201sAvm::getKey(int level, const unsigned char* seedData, int seedSize, unsigned char* keyData, int* keySize)
{
	//此处进行计算
	return true;
}

