float4 BrightPassPS(float2 coordin : TEXCOORD0) : COLOR0 {
	float3 hdrcolor = tex2D(ScreenSpaceSampler1,coordin).rgb;
	float luminance = dot(hdrcolor, float3(0.299f, 0.587f, 0.114f)); // ITU-R BT.601 luma coefficients
	float clampedluminance = max(luminance - hdrbloomthreshold, 0.0f);
	return float4(hdrcolor * clampedluminance / luminance,1);
}

technique BrightPass {
	pass P0 {
		VertexShader = compile vs_3_0 ScreenSpaceVS();
		PixelShader  = compile ps_3_0 BrightPassPS();		
	}
}
