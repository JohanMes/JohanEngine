////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Simpelweg niets anders dan 1 texture renderen, en ook zonder schaduwen
*/

struct VS_OUTPUT_PURECOLOR {
	float4 position  : POSITION0;
};

VS_OUTPUT_PURECOLOR PureColorVS(float3 positionin : POSITION0, float2 coordin : TEXCOORD0) {
	VS_OUTPUT_PURECOLOR outVS;
	
	// Bereken pixelcoordinaat (xy, z = diepte, w negeren)
	outVS.position = mul(float4(positionin,1.0f),WorldViewProj);
	
	return outVS;
}

PS_OUTPUT PureColorPS(VS_OUTPUT_PURECOLOR In) {
	PS_OUTPUT outPS;
	outPS.color = purecolor;
	return outPS;
}

technique PureColor {
	pass P0 {
		VertexShader = compile vs_3_0 PureColorVS();
		PixelShader  = compile ps_3_0 PureColorPS();
	}
}
