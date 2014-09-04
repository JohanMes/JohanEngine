float4 GaussFilterPS(float2 coordin : TEXCOORD0) : COLOR0 {
    float4 result = 0;
	for(int i = 0;i < 25;i++) {
		float2 coord = coordin + gauss[i].xy * float2(invwidth,invheight);
		result += gauss[i].z * tex2D(ScreenSpaceSampler1,coord);
	}
	return result;
}

technique GaussFilter {
	pass P0 {
		VertexShader = compile vs_3_0 ScreenSpaceVS();
		PixelShader  = compile ps_3_0 GaussFilterPS();
	}
}
