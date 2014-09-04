float4 SSAOBlurPS(float2 coordin : TEXCOORD0) : COLOR0 {
	
	// TODO: use smart blur which uses normals from texture 1
	
	// Perform the usual gaussian blur on the occlusion factor
    float blurredocclusion = 0;
	for(int i = 0;i < 25;i++) { 
		float2 coord = coordin + gauss[i].xy * float2(invwidth,invheight);
		blurredocclusion += gauss[i].z * tex2D(ScreenSpaceSampler2,coord).r; // alpha contains occlusion factor
	}
	return max(0,tex2D(ScreenSpaceSampler3,coordin) - blurredocclusion);
}

technique SSAOBlur {
	pass P0 {
		VertexShader = compile vs_3_0 ScreenSpaceVS();
		PixelShader  = compile ps_3_0 SSAOBlurPS();
	}
}
