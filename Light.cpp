#include "unit.h"
#include "Light.h"



CDirLight::CDirLight(float3 dir, float4 color)
{
	m_dir = dir;
	normalize(m_dir);
	m_color = color;
}


CDirLight::~CDirLight()
{

}
