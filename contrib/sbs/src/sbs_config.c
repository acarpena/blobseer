#include "sbs_common.h"
#include "sbs_config.h"
#include "sbs_buffer.h"
#include "sbs_types.h"

#define NODEX 4
#define NODEY 4

#define COREX 2
#define COREY 2

#define CX 20
#define CY 20
#define CZ 10

#define NX(_cx_) _cx_*COREX
#define NY(_cy_) _cy_*COREY
#define NZ CZ
#define GX(_cx_) _cx_*NODEX
#define GY(_cy_) _cy_*NODEY
#define GZ CZ

#define COUNT COREX*COREY

#define VARALLOC3D(_name_,_lsx_,_lsy_,_lsz_,_gsx_,_gsy_,_gsz_) 	\
        lsizes[0] = _lsz_ ; gsizes[0] = _gsz_ ;			\
	lsizes[1] = _lsy_ ; gsizes[1] = _gsy_ ; 		\
	lsizes[2] = _lsx_ ; gsizes[2] = _gsx_ ; 		\
        offsets[0] = 0; 					\
	offsets[1] = _lsy_*(myrank/(_gsx_/_lsx_)); 		\
	offsets[2] = _lsx_*(myrank%(_gsx_/_lsx_));		\
	sbs_buffer_define_var(_name_,SBS_FLOAT,3,lsizes,gsizes,offsets,time,COUNT);

#define VARALLOC2D(_name_,_lsx_,_lsy_,_gsx_,_gsy_)		\
	lsizes[0] = _lsy_ ; gsizes[0] = _gsy_ ;			\
	lsizes[1] = _lsx_ ; gsizes[1] = _gsx_ ;			\
	offsets[0] = _lsy_*(myrank/(_gsx_/_lsx_));		\
	offsets[1] = _lsx_*(myrank%(_gsx_/_lsx_));		\
	sbs_buffer_define_var(_name_,SBS_FLOAT,2,lsizes,gsizes,offsets,time,COUNT);
	
sbs_buffer buffer;
int myrank;

int sbs_config_initialize(int rank, sbs_buffer buf)
{
	buffer = buf;
	myrank = rank;
	return 0;
}

int sbs_config_new_step(int time)
{
	/* this is a raw configuration for testing */
	DEBUG(printf("new step configuration\n");)
	size_t lsizes[3];
	size_t gsizes[3];
	uint64_t offsets[3];

	VARALLOC2D("rain1",NX(CX),NY(CY),GX(CX),GY(CY))
	VARALLOC2D("sws1",NX(CX),NY(CY),GX(CX),GY(CY))
	VARALLOC2D("rain2",NX(CX),NY(CY),GX(CX),GY(CY))
	VARALLOC2D("sws2",NX(CX),NY(CY),GX(CX),GY(CY))

	VARALLOC3D("th",NX(CX),NY(CY),NZ,GX(CX),GY(CY),GZ)
	VARALLOC3D("prs",NX(CX),NY(CY),NZ,GX(CX),GY(CY),GZ)
	VARALLOC3D("qv",NX(CX),NY(CY),NZ,GX(CX),GY(CY),GZ)
	VARALLOC3D("uinterp",NX(CX),NY(CY),NZ,GX(CX),GY(CY),GZ)
	VARALLOC3D("vinterp",NX(CX),NY(CY),NZ,GX(CX),GY(CY),GZ)
	VARALLOC3D("winterp",NX(CX),NY(CY),NZ,GX(CX),GY(CY),GZ)
	VARALLOC3D("u",NX((CX+1)),NY(CY),NZ,GX((CX+1)),GY(CY),GZ)
	VARALLOC3D("v",NX(CX),NY((CY+1)),NZ,GX(CX),GY(CY+1),GZ)
	VARALLOC3D("w",NX(CX),NY(CY),NZ+1,GX(CX),GY(CY),GZ+1)
	VARALLOC3D("tke",NX(CX),NY(CY),NZ+1,GX(CX),GY(CY),GZ+1)
	VARALLOC3D("kmh",NX(CX),NY(CY),NZ+1,GX(CX),GY(CY),GZ+1)
	VARALLOC3D("kmv",NX(CX),NY(CY),NZ+1,GX(CX),GY(CY),GZ+1)
	VARALLOC3D("khh",NX(CX),NY(CY),NZ+1,GX(CX),GY(CY),GZ+1)
	VARALLOC3D("khv",NX(CX),NY(CY),NZ+1,GX(CX),GY(CY),GZ+1)

	return 0;
}

