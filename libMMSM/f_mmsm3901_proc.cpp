/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   石咏
Version:    1.0
Date:     2015-07-04
Description: 修磨实绩接收物料主档修改
***********************************************************************/


/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/

//#include "tmmsm01.h" 



#if defined(_SYS_PES)
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif


BM2_FUNCTION_EXPORT
int f_mmsm3901_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	int  n_count = 0;
	CString cutFinFlag = "";
	int mat_seq = 0;
	int mat_tube = 0;
	int fetchRowCount = 0;
	CString vcf_heat_no = "";//代表成分熔炼号
	CString v_remark = "";//备注
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_st_no = "";//出钢记号
	CString v_c_div = "";//碳锈区分  1  不锈钢  2碳钢
	CString new_heat_no = "";
	int count_heat_no = 0;
	CString v_operate = "";//操作区分	 I 新增    U 修改
	CString v_resume_seq_no = "";//序号
	CString v_sap_erp_matnr = "";//物料编码

	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm39("TMMSM39");
	CModel tmmsm39_1("TMMSM39_1");
	CModel tmmsm33dbsx("TMMSM33DBSX");
	CDbCommand cmd_inq(conn);

	try
	{



	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		
	}
	catch (CException& ex)
	{
		
	}


	return doFlag;

}
