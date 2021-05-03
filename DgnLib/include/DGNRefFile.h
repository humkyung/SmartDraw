#ifndef	__DGNREFFILE_H__
#define	__DGNREFFILE_H__

#include <DGNEnum.h>

typedef struct tagDGNREFFILE{
	ELMHDR		ehdr;			// element header   
	DISPHDR		dhdr;			//display header   
	short		file_chars;             // no. of chars. in file spec   
	char		file_spec[65];          //file specification   
	unsigned char	file_num;               // file number   
	Fb_opts		fb_opts;		// file builder options mask   
	Fd_opts		fd_opts;		// file displayer options mask  
	unsigned char	disp_flags[16];         // display flags   
	short		lev_flags[8][4];        // level on/off flags   
	long		ref_org[3];             // origin in ref file uors   
	double		trns_mtrx[9];           // transformation matrix   
	double		cnvrs_fact;             // conversion factor   
	long		mast_org[3];            // origin in master file uors   
	short		log_chars;              // characters in logical name   
	char		log_name[22];           // logical name (padded)   
	short		desc_chars;             // characters in description   
	char		description[42];        // description (padded)   
	short		lev_sym_mask;           // level symbology enable mask  
	short		lev_sym[63];            // level symbology descriptor   
	long		z_delta;		// Z-direction delta   
	short		clip_vertices;          // clipping vertices   
	DGNPOINT2D	clip_poly[2];		// clipping polygon =>> more than 1 =>>    
}DGNREFFILE,* PDGNREFFILE;

#endif