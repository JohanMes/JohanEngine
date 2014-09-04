VS_OUTPUT_SCREEN InterfaceVS(float3 positionin : POSITION0,float2 coordin : TEXCOORD0) {
	VS_OUTPUT_SCREEN outVS;

	// Geef een fake float4 mee, de w is altijd leeg (wordt niet geset door CPU, scheelt 1 instructie)
	outVS.position = mul(float4(positionin,1.0f),World);

	// Coords met screenpixel/texelcorrectie van een halve pixel
	outVS.coord.x = coordin.x + 0.5f * invwidth;
	outVS.coord.y = coordin.y + 0.5f * invheight;

	return outVS;
}

float4 InterfacePS() : COLOR0 {
	return interfacecolor;
}

technique Interface {
	pass P0 {
		VertexShader = compile vs_3_0 InterfaceVS();
		PixelShader  = compile ps_3_0 InterfacePS();
	}
}
