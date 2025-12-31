const uint ASDF = 234;

struct STRUCTURE {
	int aa;
	struct {
		float D;
		double C;
	} M;
	int a;
	int b;
};


void N_KERNEL_0();

__attribute__((annotate("reqd_work_group_size(2, 2, 2)")))
__kernel void KERNEL_0(
		       uint pushu32, 
		       __global uint* __attribute__((annotate("aclspv_set(2)"))) globu32, 
		       __global float* globf32, 
		       __local uint* locu32, 
		       float pushf32, 
		       struct STRUCTURE push_struct
		       ) 
{ 
	globu32[get_global_id(0)] += pushu32 + sin(globf32[get_global_id(0)]); 
	return;
}
__kernel void N_KERNEL_0(__constant int* consti32) {
	return;
}
