#include "Vehicle.h"
#include "../algorithm/aestiny.h"
#include "../algorithm/sha256.h"


uds::Sgmw201sAvm::Sgmw201sAvm(std::shared_ptr<can::Base> base, int channel, can::ProtoType proto)
	:Base(base, channel, proto)
{
	m_cid.request.physical = 0x724;
	m_cid.request.function = 0x7df;
	m_cid.request.mode = RequestMode::PHYSICAL;
	m_cid.response = 0x72c;
}

uds::Sgmw201sAvm::~Sgmw201sAvm()
{
}

bool uds::Sgmw201sAvm::getKey(int level, const unsigned char* seedData, int seedSize, unsigned char* keyData, int* keySize)
{
	// 需要自己实现解锁算法
	return true;
}

