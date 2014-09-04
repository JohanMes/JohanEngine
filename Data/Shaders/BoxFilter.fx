float4 BoxFilterPS(float2 coordin : TEXCOORD0) : COLOR0 {
	float4 result = 0;
	for(int i = 0;i < 9;i++) { // Ga een 3x3-doos rond de huidige pixel af
		float2 coord = coordin + box[i] * float2(invwidth,invheight);
		result += tex2D(ScreenSpaceSampler1,coord);
	}
	return (result/9.0f);
}

technique Box {
	pass P0 {
		VertexShader = compile vs_3_0 ScreenSpaceVS();
		PixelShader  = compile ps_3_0 BoxFilterPS();
	}
}
