float4 BrightBlurPS(float2 coordin : TEXCOORD0) : COLOR0 { // same as gauss, but at half size
    float4 result = 0;
	for(int i = 0;i < 25;i++) {
		float2 coord = coordin + 2.0f * gauss[i].xy * float2(invwidth,invheight);
		result += gauss[i].z * tex2D(ScreenSpaceSampler2,coord);
	}
	return result;
}
// Ga een doos rond de huidige pixel af
technique BrightBlur {
	pass P0 {
		VertexShader = compile vs_3_0 ScreenSpaceVS();
		PixelShader  = compile ps_3_0 BrightBlurPS();		
	}
}
