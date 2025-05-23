inlets = 1;
outlets = 1;

const mapping = {
	1: [0, 1024], //HeartRaw
	2: [0., 1.], //HeartNormalized
	3: [0, 1], //HeartBeatDetected
	4: [0., 600.], //HeartAmplitudeChange
	5: [0., 1.], //HeartBPM
	6: [0., 1.], //HeartBPMChange
	7: [0, 1024], //SkinRaw
	8: [0., 1.], //SkinSCR
	9: [0., 1.], //SkinSCL
	10: [0, 65535], //RespRaw
	11: [0., 8.], //RespNormalized
	12: [0.,1.], //RespScaled
	13: [0., 1.], //RespExhaling
	14: [0, 1000], //RespRawAmplitude
	15: [0., 8.], //RespNormalizedAmplitude
	16: [0., 1.], //RespScaledAmplitude 
	17: [0., 1.], //RespAmplitudeLevel
	18: [0., 2000.], //RespAmplitudeChange
	19: [0., 500.], //RespAmplitudeVariability
	20: [0, 1000000], //RespInterval
	21: [0., 200.], //RespRpm
	22: [0., 8.], //RespNormalizedRpm
	23: [0., 1.], //RespRpmLevel
	24: [0., 1.], //RespRpmLevel
	25: [0., 200.], //RespRpmChange
	26: [0., 500.] //RespRpmVariability
};

function msg_int(val){
	const result = mapping[val];
	if (result !== undefined) {
		outlet(0, result);
	} else {
		post("No mapping for input: " + val + "\n");
	}
}