FILE	*_SXPE_file;
int	_SXPE_col;
char	_SXPE_str [16];
char	*_SXPE_string;
extern VOID	_SXPE_pos ();

extern VOID	put_edit_fnb ();
extern VOID	put_edit_nl ();
extern VOID	put_edit_cut_point ();

#define	put_edit_ap(string)	(fputs((string),_SXPE_file),\
				 fputc ('\n',_SXPE_file),\
				 _SXPE_col=1)

#define	put_edit(col,string)	(_SXPE_pos(col),\
				 put_edit_ap(string))

#define	put_edit_apnnl(string)	(fputs((_SXPE_string=string),_SXPE_file),\
				 _SXPE_col+=strlen(_SXPE_string))

#define	put_edit_nnl(col,string)	(_SXPE_pos(col),\
					 put_edit_apnnl(string))

#define	put_edit_apnb(number)	(sprintf(_SXPE_str,"%d",(number)),\
				 fputs(_SXPE_str,_SXPE_file),\
				 _SXPE_col+=strlen(_SXPE_str))

#define	put_edit_nb(col,number)	(_SXPE_pos(col),\
				 put_edit_apnb(number))

#define	put_edit_ff()		(fputc('\f',_SXPE_file),\
				 _SXPE_col=1)

#define	put_edit_initialize(file)	(_SXPE_file=file,\
					 _SXPE_col=1)

#define	put_edit_finalize()	((_SXPE_file!=NULL)?(fclose(_SXPE_file),_SXPE_file=NULL):NULL)

#define	put_edit_get_file()	(_SXPE_file)

#define	put_edit_get_col()	(_SXPE_col)

