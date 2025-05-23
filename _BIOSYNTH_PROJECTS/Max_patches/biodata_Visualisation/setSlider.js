inlets = 1;
outlets = 1;

const mapping = {
	1: 1024, //HeartRaw
	2: 1., //HeartNormalized
	3: 1, //HeartBeatDetected
	4: 600., //HeartAmplitudeChange
	5: 1., //HeartBPM
	6: 1., //HeartBPMChange
	7: 1024, //SkinRaw
	8: 1., //SkinSCR
	9: 1., //SkinSCL
	10: 65535, //RespRaw
	11: 8., //RespNormalized
	12: 1., //RespScaled
	13: 1., //RespExhaling
	14: 1000, //RespRawAmplitude
	15: 8., //RespNormalizedAmplitude
	16: 1., //RespScaledAmplitude 
	17: 1., //RespAmplitudeLevel
	18: 2000., //RespAmplitudeChange
	19: 500., //RespAmplitudeVariability
	20: 1000000, //RespInterval
	21: 200., //RespRpm
	22: 8., //RespNormalizedRpm
	23: 1., //RespRpmLevel
	24: 1., //RespRpmLevel
	25: 200., //RespRpmChange
	26: 500. //RespRpmVariability
};

function msg_int(val){
	const result = mapping[val];
	if (result !== undefined) {
		outlet(0, result);
	} else {
		post("No mapping for input: " + val + "\n");
	}
}