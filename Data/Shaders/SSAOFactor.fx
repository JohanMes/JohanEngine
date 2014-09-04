float4 SSAOFactorPS(float2 coordin : TEXCOORD0) : COLOR0 {
	
	// Sample points around this point in VIEW space
	float4 referencepixel = tex2Dlod(ScreenSpaceSampler1,float4(coordin,0.0f,0.0f));
	
	// Screen space depth is saved in A
	float3 referencepos = GetViewPosition(float3(coordin,referencepixel.a));
	
	// Accumulate a few samples
	float occlusion = 0;
	for(int i = 0;i < ssaosamples;i++) {
		
		// Add random vector in VIEW space, don't self-occlude
		float3 neighborviewpos = referencepos;
		float3 offset = ssaoradius * ssao[i] * (GetRandomValue(coordin)*0.9f + 0.1f);
		if(dot(offset,referencepixel.rgb) < 0) {
			neighborviewpos -= offset;
		} else {
			neighborviewpos += offset;
		}
		
		// Convert back to SCREEN space, so we can compare saved depth
		float3 neighborscreenpos = GetScreenPosition(neighborviewpos);
		
		// Compare the projected depth with the saved depth...
		float depthprojected = neighborscreenpos.z;
		float depthsaved = tex2Dlod(ScreenSpaceSampler1,float4(neighborscreenpos.xy,0.0f,0.0f)).a;
		float depthdifference = depthprojected - depthsaved;
		
		// If the projected point is behind the saved one, there is occlusion
		if(depthdifference > ssaodepthbias) { // we are behind our neighbor, add some occlusion
			occlusion += exp(-ssaodepthmultiplier*(depthdifference-ssaodepthbias));
		}
	}
	occlusion = (ssaomultiplier * occlusion)/((float)ssaosamples);
	return occlusion;
}

technique SSAOFactor {
	pass P0 {
		VertexShader = compile vs_3_0 ScreenSpaceVS();
		PixelShader  = compile ps_3_0 SSAOFactorPS();
	}
}
