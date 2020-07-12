#pragma once
#include "stdint.h"
#include "stddef.h"

//#define CHANNELS 5
//#define STAGES 10
//#define UPSHIFT 10

template<int CHANNELS,int STAGES,int UPSHIFT>
class FlexMenuTransitionFilter
{
public:
	FlexMenuTransitionFilter()
	{
		uint16_t rgbcw[CHANNELS];
		for(int i=0;i<CHANNELS;i++)
		{
			rgbcw[i]=0;
		}
		preload(rgbcw);
		coeff=2;
	};

	void preload(const uint16_t * input);

	bool run(const uint16_t * input, uint16_t * output);

	int32_t buffer[STAGES][CHANNELS];

	uint8_t coeff;


};

template<int CHANNELS,int STAGES,int UPSHIFT>
void FlexMenuTransitionFilter<CHANNELS,STAGES,UPSHIFT>::preload(const uint16_t * input)
{
	for(size_t s=0;s<STAGES;s++)
	{
		for(size_t c=0;c<CHANNELS;c++)
		{
			buffer[s][c]=(uint32_t) input[c]<<UPSHIFT;
		}
	}
}

template<int CHANNELS,int STAGES,int UPSHIFT>
bool FlexMenuTransitionFilter<CHANNELS,STAGES,UPSHIFT>::run(const uint16_t * input, uint16_t * output)
{

	bool bRet=false;

	uint8_t localcoeff=coeff;

	int32_t input_up[CHANNELS];

	for(size_t c=0;c<CHANNELS;c++)
	{
		input_up[c]=(uint32_t) input[c]<<UPSHIFT;
	}


	int32_t * last=input_up;

	for(size_t s=0;s<STAGES;s++)
	{
		for(size_t c=0;c<CHANNELS;c++)
		{
			buffer[s][c] += ((last[c] - buffer[s][c])) >> localcoeff;
		}
		last=buffer[s];
	}

	for(size_t c=0;c<CHANNELS;c++)
	{
		int32_t out=(last[c] + (1<<(UPSHIFT-1)))>>UPSHIFT;
		if(out<0) out=0;
		if(out>65535) out=65535;
		output[c]=out;

		bRet |= output[c]!=input[c];
	}


	return bRet;

}



