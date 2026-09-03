/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2024
Description: 更新成分
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_updcf(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr_temp = " "; 
	CString sqlstr = "";  
	CString stat_date = " ";
	CString heat_no = "";

	CDbCommand cmd_inq(conn);  
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel tmmsm56("TMMSM56");

	try
	{
		stat_date = dateNow.SubstringNE(0, 6);
		if (bcls_rec->Tables[0].Columns.Contains("STAT_DATE"))	
		{
			stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 6);
		}
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
		{
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		}
		Log::Trace("", __FUNCTION__, "stat_date = [{0}]", stat_date);			

		//更新成分 优先级：
		//固定表成分1
		//熔清成分 TMMSMWQ
		//复验补录 		
	    //计量单成分
		//该物料最近成分 
		/*20241107暂时不需要
		//更新原料成分，为了取C\S\P
		//更新复验	
		sqlstr = " update tmmsm56 t1 set QUALITYID= (SELECT  max(QUALITY_BATCH_NO) FROM TMMSM81AH t2 WHERE t1.MAT_CODE=t2.MAT_CODE and  t1.LOT_NO=t2.LOT_NO AND REMARK_1 = 'F')"
			" where 1=1"
			" and QUALITY_BATCH_NO !=' '"
			" and exists (select 1 from  TMMSM81AH t2 where  t1.MAT_CODE=t2.MAT_CODE and  t1.LOT_NO=t2.LOT_NO AND REMARK_1 = 'F')"
			" and  stat_date = @stat_date"
			;
		if (heat_no.Trim() != "")
		{
			sqlstr = sqlstr + " and heat_no=@heat_no";
		}
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsm56 t1 set QUALITYID = (SELECT MAX(QUALITY_BATCH_NO) FROM tmmsm81ah"
			" WHERE REC_CREATE_TIME in (select max(REC_CREATE_TIME) from tmmsm81ah t2 where  t2.MAT_CODE=t1.MAT_CODE ))"
			" where 1=1"
			" and QUALITY_BATCH_NO =' ' "
			" and exists (select 1 from tmmsm81ah t2 where t1.MAT_CODE=t2.MAT_CODE) "
			" and  stat_date = @stat_date"
			;
		if (heat_no.Trim() != "")
		{
			sqlstr = sqlstr + " and heat_no=@heat_no";
		}
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsm56 t1 set (CR_VALUE,NI_VALUE,MO_VALUE,C_VALUE,SI_VALUE,S_VALUE,MN_VALUE)= (SELECT max(nvl(Cr,0)),max(nvl(Ni,0)), max(nvl(Mo,0)), max(nvl(C,0)), max(nvl(Si,0)), max(nvl(S,0)), max(nvl(Mn,0))  FROM ("
			" SELECT  * FROM ( SELECT QUALITY_BATCH_NO ,ELM_VALUE,ELM_NAME FROM  TMMSM81AL )"
			" PIVOT ( SUM(ELM_VALUE) FOR ELM_NAME IN ( 'Cr' AS Cr ,'Ni' AS Ni, 'Mo' AS Mo, 'C' AS C , 'S' AS S, 'Si' AS Si, 'Mn' AS Mn))) t2  where t1.QUALITYID=t2.QUALITY_BATCH_NO GROUP BY  QUALITY_BATCH_NO)"
			" where 1=1"
			" and exists (select 1 from  TMMSM81AL t2 where t1.QUALITYID=t2.QUALITY_BATCH_NO)"
			" and  stat_date = @stat_date"
			;
		if (heat_no.Trim() != "")
		{
			sqlstr = sqlstr + " and heat_no=@heat_no";
		}
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsm56 t1 set (CR_VALUE,NI_VALUE,MO_VALUE,C_VALUE,SI_VALUE,S_VALUE,MN_VALUE)= (SELECT max(nvl(Cr,0)),max(nvl(Ni,0)), max(nvl(Mo,0)), max(nvl(C,0)), max(nvl(Si,0)), max(nvl(S,0)), max(nvl(Mn,0))  FROM ("
			" SELECT  * FROM ( SELECT QUALITY_BATCH_NO ,ELM_VALUE,ELM_NAME FROM  TMMSM81AL )"
			" PIVOT ( SUM(ELM_VALUE) FOR ELM_NAME IN ( 'Cr' AS Cr ,'Ni' AS Ni, 'Mo' AS Mo, 'C' AS C , 'S' AS S, 'Si' AS Si, 'Mn' AS Mn))) t2  where t1.QUALITY_BATCH_NO=t2.QUALITY_BATCH_NO GROUP BY  QUALITY_BATCH_NO)"
			" where 1=1"
			" and QUALITYID =' '"
			" and exists (select 1 from  TMMSM81AL t2 where t1.QUALITY_BATCH_NO=t2.QUALITY_BATCH_NO)"
			" and  stat_date = @stat_date"
			;
		if (heat_no.Trim() != "")
		{
			sqlstr = sqlstr + " and heat_no=@heat_no";
		}
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 

		//熔清
		sqlstr = " update tmmsm56 t1 set  (QUALITYID,CR_VALUE,NI_VALUE,MO_VALUE,C_VALUE,SI_VALUE,S_VALUE,MN_VALUE) = (select LOT_NO,nvl(CR_VALUE,0),nvl(NI_VALUE,0),nvl(MO_VALUE,0),nvl(C_VALUE,0),nvl(SI_VALUE,0),nvl(S_VALUE,0),nvl(MN_VALUE,0) from TMMSMWQ t2 where t1.LOT_NO=t2.LOT_NO)"
			" where 1=1"
			" and exists (select 1 from TMMSMWQ t2 where t1.LOT_NO=t2.LOT_NO)"
			" and  stat_date = @stat_date"
			;
		if (heat_no.Trim() != "")
		{
			sqlstr = sqlstr + " and heat_no=@heat_no";
		}
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 

		//固定表（ZJ_MAT_ELEMENT类别：1.按表内成分填写，2用成分乘以表中系数，3只用表内Cr成分，4只用表中Ni成分）
		sqlstr = " update tmmsm56 t1 set (QUALITYID,CR_VALUE,NI_VALUE,MO_VALUE) = (select 'ZJ_MAT_ELEMENT',CR,NI,MO from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID  and t2.TYPE='1') "
			" where 1=1"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='1')"
			" and  stat_date = @stat_date"
			;
		if (heat_no.Trim() != "")
		{
			sqlstr = sqlstr + " and heat_no=@heat_no";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsm56 t1 set (CR_VALUE) = (select CR from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID)"
			" where 1=1"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='3')"
			" and  stat_date = @stat_date"
			;
		if (heat_no.Trim() != "")
		{
			sqlstr = sqlstr + " and heat_no=@heat_no";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsm56 t1 set (NI_VALUE) = (select NI from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID)"
			" where 1=1"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='4')"
			" and  stat_date = @stat_date"
			;
		if (heat_no.Trim() != "")
		{
			sqlstr = sqlstr + " and heat_no=@heat_no";
		}
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsm56 t1 set (MO_VALUE) = (select MO from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID)"
			" where 1=1"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='5')"
			" and  stat_date = @stat_date"
			;
		if (heat_no.Trim() != "")
		{
			sqlstr = sqlstr + " and heat_no=@heat_no";
		}
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsm56 t1 set (CR_VALUE,NI_VALUE) = (select CR*t1.CR_VALUE,NI*t1.NI_VALUE from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID) "
			" where 1=1"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='2')"
			" and  stat_date = @stat_date"
			;
		if (heat_no.Trim() != "")
		{
			sqlstr = sqlstr + " and heat_no=@heat_no";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		*/

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}
