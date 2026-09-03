/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 投料明细报表
出钢时间：转炉/AOD结束时间
开浇时间：连铸打包开始时间
关包时间：连铸结束时间
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmtlmx_inq)

int f_mmsmtlmx_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int count = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString end_time = " ";
	CString begin_time = " ";
	CString heat_no = "";
	CString mat_code = "";
	CString gd_flag = "0";
	CString c_div = "";
	CString cast_div_no = "";
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	//系统的分页类信息。
	CPageInfo pageInfo;



	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	try
	{
		try{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}


		//--------------------------------
		//获取传入参数

		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("DEVO_TIME"))
			begin_time = bcls_rec->Tables[0].Rows[0]["DEVO_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
			mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("GD_FLAG"))
			gd_flag = bcls_rec->Tables[0].Rows[0]["GD_FLAG"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("C_DIV"))
			c_div = bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("CAST_DIV_NO"))
			cast_div_no= bcls_rec->Tables[0].Rows[0]["CAST_DIV_NO"].ToString(); 

		if (begin_time.Trim() == "" || end_time.Trim()=="")
		{
			strcpy(s.msg, "开始时间或结束时间不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}


		if (bcls_rec->Tables.Contains("PAGEINFO"))
		{
			if (bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_NUM") && bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_SIZE"))
			{
				current_page_no = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_NUM"].ToDecimal().ToInt32() + 1;
				record_count_per_page = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_SIZE"];
			}
			else {
				record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
				current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
			}
		}
		else {
			record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
			current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
		}

		sqlstr_count = " SELECT COUNT(1) FROM "
			" (SELECT  F.HEAT_NO,CAST_DIV_NO FROM TMMSM31 F "
			" left join ("
			" select heat_no,mat_code,dev_code,QUALITY_BATCH_NO,LOT_NO,sum(DEVO_WT*0.001) DEVO_WT "
			" from tmmsmgy08 A"
			" where 1=1"
			//" and MAT_CODE not in ( SELECT MAT_CODE FROM TMMSM50 WHERE SEND_FLAG = '1')"
			" and heat_no in (select heat_no from tmmsm21 t2 where st_no != 'DeP' and st_no not like '1%'  and  end_time<=@end_time and end_time>=@begin_time union  select heat_no from tmmsm27 t2 where end_time<=@end_time and end_time>=@begin_time)"
			;
		if (heat_no.Trim() != "")
		{
			sqlstr_count += " AND HEAT_NO  like @heat_no||'%'";
		}
		;
		sqlstr_count += " group by heat_no,mat_code,dev_code,QUALITY_BATCH_NO,LOT_NO "
			" ) t2 on F.HEAT_NO=t2.heat_no"
			" where 1=1"
			" and F.heat_no in (select heat_no from tmmsm21 t2 where st_no != 'DeP' and st_no not like '1%'  and  end_time<=@end_time and end_time>=@begin_time union  select heat_no from tmmsm27 t2 where end_time<=@end_time and end_time>=@begin_time)"
			;
		if (heat_no.Trim() != "")
		{
			sqlstr_count += " AND F.HEAT_NO  like @heat_no||'%'";
		}
		if (c_div.Trim() == "1")  //不锈钢
		{
			sqlstr_count += " and substr(F.ST_NO,1,1) in ('1','4')";
		}
		if (c_div.Trim() == "2")  //碳钢钢
		{
			sqlstr_count += " and substr(F.ST_NO,1,1) in ('2','3','5')";
		}
		if (mat_code.Trim() != "")
		{
			sqlstr_count += " AND t2.MAT_CODE =@mat_code";
		}
		if (cast_div_no.Trim() != "")
		{
			sqlstr_count += " AND CAST_DIV_NO =@cast_div_no";
		}
		sqlstr_count += " )";  		

		cmd_inq.SetCommandText(sqlstr_count);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.Parameters.Set("mat_code", mat_code);
		cmd_inq.Parameters.Set("cast_div_no", cast_div_no);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		cmd_inq.Close();
		
		if (gd_flag != "1")
		{ 
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default: 
				sqlstr = " select t1.heat_no,TAP_TIME AS LADLE_ARRIVE_TIME,TD_REMAIN_WT,LADLE_OPEN_TIME,LADLE_CLOSE_TIME,TD_NO_1,MOLD_NO1,CAST_DIV_NO "
					",ST_NO ,OUT_STEEL_WT ,ST_NO_CLASS,ROUTELIST,MAT_ACT_WT,ST_NO_DESC,RAW_WEIGHT "
					",nvl((select sum(CUT_SCRAP_WT) from tmmsmfp t3 where  t3.heat_no=t1.heat_no ),0) CUT_SCRAP_WT"
					", nvl((select sum(mat_act_wt) from hmmsm01 t3 where t3.complex_decide_code = '9' and t3.heat_no = t1.heat_no), 0) AS ALLOY_FG"
					",C,SI,MN,P,S,CR,NI,MO,CU,AI,NB,V,TI,N "
					", A.mat_code, A.dev_code,  A.QUALITY_BATCH_NO,A.DEVO_WT ,A.LOT_NO"
					",NVL((SELECT MAT_NAME FROM TMMSM50 T WHERE T.MAT_CODE = A.MAT_CODE ),' ') MAT_NAME "
					" , nvl(E.MAT_TYPE_DESC,' ')  as TYPE_DL  "
					" ,NVL((SELECT MAX(ELM_VALUE)  FROM TMMSM81AL  WHERE   ELM_CODE='18' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_C"
					" ,NVL((SELECT MAX(ELM_VALUE)  FROM TMMSM81AL  WHERE   ELM_CODE='144' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_SI"
					" ,NVL((SELECT MAX(ELM_VALUE)  FROM TMMSM81AL  WHERE   ELM_CODE='122' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_P"
					" ,NVL((SELECT MAX(ELM_VALUE)  FROM TMMSM81AL  WHERE   ELM_CODE='134' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_S"
					" ,NVL((SELECT MAX(ELM_VALUE)  FROM TMMSM81AL  WHERE   ELM_CODE='46' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_CR"
					" ,NVL((SELECT MAX(ELM_VALUE)  FROM TMMSM81AL  WHERE   ELM_CODE='117' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_NI"
					" ,NVL((SELECT MAX(ELM_VALUE)  FROM TMMSM81AL  WHERE   ELM_CODE='103' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_MO"
					" ,NVL((SELECT MAX(ELM_VALUE)  FROM TMMSM81AL  WHERE   ELM_CODE='96' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_MN "
					" from "
					"  (select  F.heat_no,TD_REMAIN_WT,END_TIME AS LADLE_CLOSE_TIME,TD_NO_1,MOLD_NO1,CAST_DIV_NO  "
					", F.ST_NO ,F.LADLE_ARRIVE_WT-F.LADLE_LEAVE_WT  OUT_STEEL_WT ,LADLE_OPEN_TIME"
					", (CASE WHEN SUBSTR(F.ST_NO, 0, 2) = '1A' OR SUBSTR(F.ST_NO, 0, 2) = '1D' THEN '镍钢' WHEN SUBSTR(F.ST_NO, 0, 2) = '1M' OR SUBSTR(F.ST_NO, 0, 2) = '1F' THEN '铬钢' WHEN SUBSTR(F.ST_NO, 0, 1) = '1' THEN '不锈钢' else '碳钢' end) ST_NO_CLASS"
					"   ,nvl(((SELECT  LISTAGG(DISTINCT DEV_CODE,'') WITHIN GROUP (ORDER BY START_TIME) FROM TMMSMGY06  t2 where  t2.heat_no=F.heat_no)), '') as ROUTELIST  "
					",nvl((select SUM(MAT_ACT_WT) FROM VMMSMCPCL_BB B where B.heat_no = F.heat_no), 0) AS MAT_ACT_WT"
					",nvl((select SUM(RECEIVE_WEIGHT) FROM VMMSMCPCL_BB B where B.heat_no = F.heat_no), 0) AS RAW_WEIGHT"
					",nvl((select MAX(ST_NO_DESC) FROM TQMTS0X C where C.ST_NO = F.ST_NO), ' ') AS ST_NO_DESC"
					", nvl(CASE WHEN H.ELM_001 = -1 THEN 0 ELSE ELM_001 END, 0) C "
					", nvl(CASE WHEN H.ELM_002 = -1 THEN 0 ELSE ELM_002 END, 0) SI "
					", nvl(CASE WHEN H.ELM_003 = -1 THEN 0 ELSE ELM_003 END, 0) MN "
					", nvl(CASE WHEN H.ELM_004 = -1 THEN 0 ELSE ELM_004 END, 0) P  "
					", nvl(CASE WHEN H.ELM_005 = -1 THEN 0 ELSE ELM_005 END, 0) S "
					", nvl(CASE WHEN H.ELM_006 = -1 THEN 0 ELSE ELM_006 END, 0) CR	"
					", nvl(CASE WHEN H.ELM_007 = -1 THEN 0 ELSE ELM_007 END, 0) NI "
					", nvl(CASE WHEN H.ELM_008 = -1 THEN 0 ELSE ELM_008 END, 0) MO "
					", nvl(CASE WHEN H.ELM_009 = -1 THEN 0 ELSE ELM_009 END, 0) CU "
					", nvl(CASE WHEN H.ELM_010 = -1 THEN 0 ELSE ELM_010 END, 0) AI "
					", nvl(CASE WHEN H.ELM_011 = -1 THEN 0 ELSE ELM_011 END, 0) NB "
					", nvl(CASE WHEN H.ELM_012 = -1 THEN 0 ELSE ELM_012 END, 0) V  "
					", nvl(CASE WHEN H.ELM_013 = -1 THEN 0 ELSE ELM_013 END, 0) TI	"
					", nvl(CASE WHEN H.ELM_016 = -1 THEN 0 ELSE ELM_016 END, 0) N	"
					",nvl((SELECT MAX(end_time) end_time FROM(select end_time from tmmsm21 t2 where t2.heat_no = F.heat_no union select end_time from tmmsm27 t2 where  t2.heat_no = F.heat_no)), ' ') TAP_TIME"
					" from tmmsm31  F left join TQMTSB0 H on F.HEAT_NO=H.HEAT_NO"
					" where 1=1"
					" and F.heat_no in (select heat_no from tmmsm21 t2 where st_no != 'DeP' and st_no not like '1%'  and  end_time<=@end_time and end_time>=@begin_time union  select heat_no from tmmsm27 t2 where end_time<=@end_time and end_time>=@begin_time)"
					;
				if (heat_no.Trim() != "")
				{
					sqlstr += " AND F.HEAT_NO  like @heat_no||'%'";
				}
				if (c_div.Trim() == "1")  //不锈钢
				{
					sqlstr += " and substr(F.ST_NO,1,1) in ('1','4')";
				}
				if (c_div.Trim() == "2")  //碳钢钢
				{
					sqlstr += " and substr(F.ST_NO,1,1) in ('2','3','5')";
				}
				sqlstr += ") t1 left join ("
					" select heat_no,mat_code,dev_code,QUALITY_BATCH_NO,LOT_NO,sum(DEVO_WT*0.001) DEVO_WT"
					" from tmmsmgy08 A"
					" where 1=1"
					//" and MAT_CODE not in ( SELECT MAT_CODE FROM TMMSM50 WHERE SEND_FLAG = '1')"
					" and exists (select 1 from tmmsm31 t2 where A.heat_no = t2.heat_no) "
					" and heat_no in (select heat_no from tmmsm21 t2 where st_no != 'DeP' and st_no not like '1%'  and  end_time<=@end_time and end_time>=@begin_time union  select heat_no from tmmsm27 t2 where end_time<=@end_time and end_time>=@begin_time)"
					" group by heat_no,mat_code,dev_code,QUALITY_BATCH_NO,LOT_NO "
					" ) A on t1.heat_no=A.heat_no"
					" left join tqmtscb08_dr E ON E.MAT_CODE_DR = A.MAT_CODE "
					" WHERE 1=1"
					;
				
				if (mat_code.Trim() != "")
				{
					sqlstr += " AND nvl(A.MAT_CODE,' ') =@mat_code";
				}
				if (cast_div_no.Trim() != "")
				{
					sqlstr += " AND nvl(CAST_DIV_NO,' ') =@cast_div_no";
				}
				break;
			} 			
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("mat_code", mat_code);
			cmd_inq.Parameters.Set("cast_div_no", cast_div_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
			cmd_inq.Close();

			for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
			{
				if (bcls_ret->Tables[0].Rows[i]["QUALITY_BATCH_NO"].ToString().Trim() == "")
				{
					sqlstr = "SELECT  *  FROM (SELECT   MAT_CODE, ELM_NAME,ELM_VALUE  FROM  ( SELECT MAT_CODE, ELM_NAME,ELM_VALUE  FROM TMMSM81AL "
						" WHERE  QUALITY_BATCH_NO = (SELECT MAX(QUALITY_BATCH_NO) FROM tmmsm81ah "
						"WHERE REC_CREATE_TIME in (select max(REC_CREATE_TIME) from tmmsm81ah where  MAT_CODE=@tmmsm81al.MAT_CODE ) and MAT_CODE=@tmmsm81al.MAT_CODE ))) "
						"pivot(MAX(ELM_VALUE)   FOR ELM_NAME IN('C' AS MAT_C,'Si'AS MAT_SI,'P'AS MAT_P,'S'AS MAT_S,'Cr'AS MAT_CR,'Ni'AS MAT_NI, "
						" 'Mn'AS MAT_MN, 'Mo'AS MAT_MO)) "
						;

					Log::Trace(" ", __FUNCTION__, "sqlstr1=[{0}]", sqlstr);
					cmd_inq1.SetCommandText(sqlstr);
					cmd_inq1.Parameters.Clear();
					cmd_inq1.Parameters.Set("tmmsm81al.MAT_CODE", bcls_ret->Tables[0].Rows[i]["MAT_CODE"].ToString().Trim());

					cmd_inq1.ExecuteReader();
					if (cmd_inq1.Read())
					{
						bcls_ret->Tables[0].Rows[i]["MAT_C"] = cmd_inq1.GetDecimal(2);
						bcls_ret->Tables[0].Rows[i]["MAT_SI"] = cmd_inq1.GetDecimal(3);
						bcls_ret->Tables[0].Rows[i]["MAT_P"] = cmd_inq1.GetDecimal(4);
						bcls_ret->Tables[0].Rows[i]["MAT_S"] = cmd_inq1.GetDecimal(5);
						bcls_ret->Tables[0].Rows[i]["MAT_CR"] = cmd_inq1.GetDecimal(6);
						bcls_ret->Tables[0].Rows[i]["MAT_NI"] = cmd_inq1.GetDecimal(7);
						bcls_ret->Tables[0].Rows[i]["MAT_MN"] = cmd_inq1.GetDecimal(8);
						bcls_ret->Tables[0].Rows[i]["MAT_MO"] = cmd_inq1.GetDecimal(9);
					}

					cmd_inq1.Close();
				}
			}

			//返回分页总数量信息 
			bcls_ret->Tables.Add("PAGEINFO");	//增加块
			bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
			bcls_ret->Tables["PAGEINFO"].Rows.Add();
			bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
		}
		else
		{
			/*sqlstr = " select t1.heat_no,TAP_TIME AS LADLE_ARRIVE_TIME,TD_REMAIN_WT,LADLE_OPEN_TIME,LADLE_CLOSE_TIME,TD_NO_1,MOLD_NO1,CAST_DIV_NO "
				",ST_NO ,OUT_STEEL_WT ,ST_NO_CLASS,ROUTELIST,MAT_ACT_WT,ST_NO_DESC,RAW_WEIGHT "
				",nvl((select sum(CUT_SCRAP_WT) from tmmsmfp t3 where  t3.heat_no=t1.heat_no ),0) CUT_SCRAP_WT"
				", nvl((select sum(mat_act_wt) from hmmsm01 t3 where t3.complex_decide_code = '9' and t3.heat_no = t1.heat_no), 0) AS ALLOY_FG"
				",C,SI,MN,P,S,CR,NI,MO,CU,AI,NB,V,TI,N "
				", A.mat_code, A.dev_code,  A.QUALITY_BATCH_NO,A.DEVO_WT ,A.LOT_NO"
				",NVL((SELECT MAT_NAME FROM TMMSM50 T WHERE T.MAT_CODE = A.MAT_CODE ),' ') MAT_NAME "
				" , nvl(E.MAT_TYPE_DESC,' ')  as TYPE_DL  "
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='C' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_C"
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='Si' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_SI"
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='Mn' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_MN "
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='P' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_P"
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='S' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_S"
				", case when lot_no in(select lot_no from TMMSMWQ) then round((select max(cr_VALUE) from  TMMSMWQ where lot_no = A.lot_no)*nvl((select INCLUDE_CR from TQMTSCB11_DR where mat_code = A.MAT_CODE),0),4)"
				"     when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '3') then(select cr from  ZJ_MAT_ELEMENT where TYPE = '3' and MAT_ID = A.MAT_CODE)"
				"     when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '1') then(select cr from  ZJ_MAT_ELEMENT where TYPE = '1' and MAT_ID = A.MAT_CODE)"				
				"     when QUALITY_BATCH_NO != ' ' then nvl((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE = A.MAT_CODE  AND ELM_NAME = 'Cr' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO), 0)	"
				" else nvl((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE = A.MAT_CODE  AND ELM_NAME = 'Cr' and QUALITY_BATCH_NO in(SELECT MAX(QUALITY_BATCH_NO) FROM tmmsm81ah	"
				"          WHERE REC_CREATE_TIME in(select max(REC_CREATE_TIME) from tmmsm81ah t2 where  t2.MAT_CODE = A.MAT_CODE))), 0)"
				" END MAT_CR"
				" ,case when lot_no in(select lot_no from TMMSMWQ) then round((select max(Ni_VALUE) from  TMMSMWQ where lot_no = A.lot_no)*nvl((select INCLUDE_NI from TQMTSCB11_DR where mat_code = A.MAT_CODE),0),4)"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '4') then(select NI from  ZJ_MAT_ELEMENT where TYPE = '4' and MAT_ID = A.MAT_CODE)"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '1') then(select Ni from  ZJ_MAT_ELEMENT where TYPE = '1' and MAT_ID = A.MAT_CODE) "
				"  when QUALITY_BATCH_NO != ' ' then nvl((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE = A.MAT_CODE  AND ELM_NAME = 'Ni' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO), 0)"
				" else nvl((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE = A.MAT_CODE  AND ELM_NAME = 'Ni' and QUALITY_BATCH_NO in(SELECT MAX(QUALITY_BATCH_NO) FROM tmmsm81ah	 "
				"     WHERE REC_CREATE_TIME in(select max(REC_CREATE_TIME) from tmmsm81ah t2 where  t2.MAT_CODE = A.MAT_CODE))), 0)"
				" END MAT_NI"
				" ,case when lot_no in(select lot_no from TMMSMWQ) then round((select max(Mo_VALUE) from  TMMSMWQ where lot_no = A.lot_no)*nvl((select INCLUDE_MO from TQMTSCB11_DR where mat_code = A.MAT_CODE),0),4)"
				"  when mat_code in (select MAT_ID from ZJ_MAT_ELEMENT where TYPE='5') then (select Mo from  ZJ_MAT_ELEMENT where TYPE='5' and MAT_ID = A.MAT_CODE)"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '1') then(select Mo from  ZJ_MAT_ELEMENT where TYPE = '1' and MAT_ID = A.MAT_CODE)"
				"  when QUALITY_BATCH_NO != ' ' then nvl((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE = A.MAT_CODE  AND ELM_NAME = 'Mo' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO), 0)"
				" else nvl((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE = A.MAT_CODE  AND ELM_NAME = 'Mo' and QUALITY_BATCH_NO in(SELECT MAX(QUALITY_BATCH_NO) FROM tmmsm81ah	"
				"     WHERE REC_CREATE_TIME in(select max(REC_CREATE_TIME) from tmmsm81ah t2 where  t2.MAT_CODE = A.MAT_CODE))), 0)"
				" END MAT_MO "
				" ,case when lot_no in(select lot_no from TMMSMWQ) then '熔清'"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '1') then '固定1'"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '2') then '固定2'"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '3') then '固定3'"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '4') then '固定4'"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '5') then '固定5'"
				"  when QUALITY_BATCH_NO != ' ' and QUALITY_BATCH_NO  in (select QUALITY_BATCH_NO FROM TMMSM81AH t2 WHERE  REMARK_1 = 'F' ) then '复验'"
				"  when QUALITY_BATCH_NO != ' ' and QUALITY_BATCH_NO  in (select QUALITY_BATCH_NO FROM TMMSM81AH t2 WHERE  REMARK_1!= 'F' ) then '质检批次'"
				"  else '该物料最近成分'"
				"  END TYPE_DESC "
				" from "
				"  (select  F.heat_no,TD_REMAIN_WT,END_TIME AS LADLE_CLOSE_TIME,TD_NO_1,MOLD_NO1,CAST_DIV_NO  "
				", F.ST_NO ,F.LADLE_ARRIVE_WT-F.LADLE_LEAVE_WT  OUT_STEEL_WT ,LADLE_OPEN_TIME"
				", (CASE WHEN SUBSTR(F.ST_NO, 0, 2) = '1A' OR SUBSTR(F.ST_NO, 0, 2) = '1D' THEN '镍钢' WHEN SUBSTR(F.ST_NO, 0, 2) = '1M' OR SUBSTR(F.ST_NO, 0, 2) = '1F' THEN '铬钢' WHEN SUBSTR(F.ST_NO, 0, 1) = '1' THEN '不锈钢' else '碳钢' end) ST_NO_CLASS"
				"   ,nvl(((SELECT  LISTAGG(DISTINCT DEV_CODE,'') WITHIN GROUP (ORDER BY START_TIME) FROM TMMSMGY06  t2 where  t2.heat_no=F.heat_no)), '') as ROUTELIST  "
				",nvl((select SUM(MAT_ACT_WT) FROM VMMSMCPCL_BB B where B.heat_no = F.heat_no), 0) AS MAT_ACT_WT"
				",nvl((select SUM(RECEIVE_WEIGHT) FROM VMMSMCPCL_BB B where B.heat_no = F.heat_no), 0) AS RAW_WEIGHT"
				",nvl((select MAX(ST_NO_DESC) FROM TQMTS0X C where C.ST_NO = F.ST_NO), ' ') AS ST_NO_DESC"
				", nvl(CASE WHEN H.ELM_001 = -1 THEN 0 ELSE ELM_001 END, 0) C "
				", nvl(CASE WHEN H.ELM_002 = -1 THEN 0 ELSE ELM_002 END, 0) SI "
				", nvl(CASE WHEN H.ELM_003 = -1 THEN 0 ELSE ELM_003 END, 0) MN "
				", nvl(CASE WHEN H.ELM_004 = -1 THEN 0 ELSE ELM_004 END, 0) P  "
				", nvl(CASE WHEN H.ELM_005 = -1 THEN 0 ELSE ELM_005 END, 0) S "
				", nvl(CASE WHEN H.ELM_006 = -1 THEN 0 ELSE ELM_006 END, 0) CR	"
				", nvl(CASE WHEN H.ELM_007 = -1 THEN 0 ELSE ELM_007 END, 0) NI "
				", nvl(CASE WHEN H.ELM_008 = -1 THEN 0 ELSE ELM_008 END, 0) MO "*/
				//", nvl(CASE WHEN H.ELM_009 = -1 THEN 0 ELSE ELM_009 END, 0) CU "
				//", nvl(CASE WHEN H.ELM_010 = -1 THEN 0 ELSE ELM_010 END, 0) AI "
				//", nvl(CASE WHEN H.ELM_011 = -1 THEN 0 ELSE ELM_011 END, 0) NB "
				//", nvl(CASE WHEN H.ELM_012 = -1 THEN 0 ELSE ELM_012 END, 0) V  "
				//", nvl(CASE WHEN H.ELM_013 = -1 THEN 0 ELSE ELM_013 END, 0) TI	"
				//", nvl(CASE WHEN H.ELM_016 = -1 THEN 0 ELSE ELM_016 END, 0) N	"
				//",nvl((SELECT MAX(end_time) end_time FROM(select end_time from tmmsm21 t2 where t2.heat_no = F.heat_no union select end_time from tmmsm27 t2 where  t2.heat_no = F.heat_no)), ' ') TAP_TIME"
				//" from tmmsm31  F left join TQMTSB0 H on F.HEAT_NO=H.HEAT_NO"
				//" where 1=1"
				//" and F.heat_no in (select heat_no from tmmsm21 t2 where st_no != 'DeP' and st_no not like '1%'  and  end_time<=@end_time and end_time>=@begin_time union  select heat_no from tmmsm27 t2 where end_time<=@end_time and end_time>=@begin_time)"
				//;

			sqlstr = " select t1.heat_no,TAP_TIME AS LADLE_ARRIVE_TIME,TD_REMAIN_WT,LADLE_OPEN_TIME,LADLE_CLOSE_TIME,TD_NO_1,MOLD_NO1,CAST_DIV_NO,'CCM' AS FROM_DESC"
				",ST_NO ,OUT_STEEL_WT ,ST_NO_CLASS,ROUTELIST,MAT_ACT_WT,ST_NO_DESC,RAW_WEIGHT "
				",nvl((select sum(CUT_SCRAP_WT) from tmmsmfp t3 where  t3.heat_no=t1.heat_no ),0) CUT_SCRAP_WT"
				", nvl((select sum(mat_act_wt) from hmmsm01 t3 where t3.complex_decide_code = '9' and t3.heat_no = t1.heat_no), 0) AS ALLOY_FG"
				",C,SI,MN,P,S,CR,NI,MO,CU,AI,NB,V,TI,N "
				", A.mat_code, A.dev_code,  A.QUALITY_BATCH_NO,A.DEVO_WT ,A.LOT_NO"
				",NVL((SELECT MAT_NAME FROM TMMSM50 T WHERE T.MAT_CODE = A.MAT_CODE ),' ') MAT_NAME "
				" , nvl(E.MAT_TYPE_DESC,' ')  as TYPE_DL  "
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='C' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_C"
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='Si' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_SI"
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='Mn' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_MN "
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='P' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_P"
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='S' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_S"
				", case when lot_no in(select lot_no from TMMSMWQ) then round((select max(cr_VALUE) from (SELECT LOT_NO, cr_VALUE, row_number() over (partition by LOT_NO order by REC_CREATE_TIME desc) as rn FROM TMMSMWQ where lot_no = A.lot_no) where rn = 1)*nvl((select INCLUDE_CR from TQMTSCB11_DR where mat_code = A.MAT_CODE),0),4)"
				"     when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '3') then(select cr from  ZJ_MAT_ELEMENT where TYPE = '3' and MAT_ID = A.MAT_CODE)"
				"     when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '1') then(select cr from  ZJ_MAT_ELEMENT where TYPE = '1' and MAT_ID = A.MAT_CODE)"
				" when lot_no in (SELECT lot_no FROM TMMSM81AH t2 WHERE t2.MAT_CODE =A.MAT_CODE AND REMARK_1 = 'F' and t2.lot_no=A.lot_no) then nvl((select ELM_VALUE from (SELECT LOT_NO, ELM_VALUE, row_number() over(partition by LOT_NO order by REC_CREATE_TIME desc) as rn FROM (SELECT LOT_NO, ELM_VALUE, REC_CREATE_TIME FROM TMMSM81AL WHERE MAT_CODE = A.MAT_CODE AND ELM_NAME = 'Cr' and QUALITY_BATCH_NO =(SELECT QUALITY_BATCH_NO from (SELECT QUALITY_BATCH_NO,row_number() over(partition by LOT_NO order by REC_CREATE_TIME desc) as rn  FROM TMMSM81AH t2 WHERE t2.MAT_CODE =A.MAT_CODE AND REMARK_1= 'F' and t2.lot_no=A.lot_no) where rn = 1))) where rn = 1), 0)"
				"     when QUALITY_BATCH_NO != ' ' then nvl((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE = A.MAT_CODE  AND ELM_NAME = 'Cr' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO), 0)	"
				" else nvl((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE = A.MAT_CODE  AND ELM_NAME = 'Cr' and QUALITY_BATCH_NO in(SELECT MAX(QUALITY_BATCH_NO) FROM tmmsm81ah	"
				"          WHERE REC_CREATE_TIME in(select max(REC_CREATE_TIME) from tmmsm81ah t2 where  t2.MAT_CODE = A.MAT_CODE))), 0)"
				" END MAT_CR"
				" ,case when lot_no in(select lot_no from TMMSMWQ) then round((select max(Ni_VALUE) from (SELECT LOT_NO, Ni_VALUE, row_number() over (partition by LOT_NO order by REC_CREATE_TIME desc) as rn  FROM TMMSMWQ where lot_no = A.lot_no) where rn = 1)*nvl((select INCLUDE_NI from TQMTSCB11_DR where mat_code = A.MAT_CODE),0),4)"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '4') then(select NI from  ZJ_MAT_ELEMENT where TYPE = '4' and MAT_ID = A.MAT_CODE)"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '1') then(select Ni from  ZJ_MAT_ELEMENT where TYPE = '1' and MAT_ID = A.MAT_CODE) "
				" when lot_no in (SELECT lot_no FROM TMMSM81AH t2 WHERE t2.MAT_CODE =A.MAT_CODE AND REMARK_1 = 'F' and t2.lot_no=A.lot_no) then nvl((select ELM_VALUE from (SELECT LOT_NO, ELM_VALUE, row_number() over(partition by LOT_NO order by REC_CREATE_TIME desc) as rn FROM (SELECT LOT_NO, ELM_VALUE, REC_CREATE_TIME FROM TMMSM81AL WHERE MAT_CODE = A.MAT_CODE AND ELM_NAME = 'Ni' and QUALITY_BATCH_NO =(SELECT QUALITY_BATCH_NO from (SELECT QUALITY_BATCH_NO,row_number() over(partition by LOT_NO order by REC_CREATE_TIME desc) as rn  FROM TMMSM81AH t2 WHERE t2.MAT_CODE =A.MAT_CODE AND REMARK_1= 'F' and t2.lot_no=A.lot_no) where rn = 1))) where rn = 1), 0)"
				"  when QUALITY_BATCH_NO != ' ' then nvl((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE = A.MAT_CODE  AND ELM_NAME = 'Ni' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO), 0)"
				" else nvl((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE = A.MAT_CODE  AND ELM_NAME = 'Ni' and QUALITY_BATCH_NO in(SELECT MAX(QUALITY_BATCH_NO) FROM tmmsm81ah	 "
				"     WHERE REC_CREATE_TIME in(select max(REC_CREATE_TIME) from tmmsm81ah t2 where  t2.MAT_CODE = A.MAT_CODE))), 0)"
				" END MAT_NI"
				" ,case when lot_no in(select lot_no from TMMSMWQ) then round((select max(Mo_VALUE) from (SELECT LOT_NO, Mo_VALUE, row_number() over (partition by LOT_NO order by REC_CREATE_TIME desc) as rn  FROM TMMSMWQ where lot_no = A.lot_no) where rn = 1)*nvl((select INCLUDE_MO from TQMTSCB11_DR where mat_code = A.MAT_CODE),0),4)"
				"  when mat_code in (select MAT_ID from ZJ_MAT_ELEMENT where TYPE='5') then (select Mo from  ZJ_MAT_ELEMENT where TYPE='5' and MAT_ID = A.MAT_CODE)"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '1') then(select Mo from  ZJ_MAT_ELEMENT where TYPE = '1' and MAT_ID = A.MAT_CODE)"
				" when lot_no in (SELECT lot_no FROM TMMSM81AH t2 WHERE t2.MAT_CODE =A.MAT_CODE AND REMARK_1 = 'F' and t2.lot_no=A.lot_no) then nvl((select ELM_VALUE from (SELECT LOT_NO, ELM_VALUE, row_number() over(partition by LOT_NO order by REC_CREATE_TIME desc) as rn FROM (SELECT LOT_NO, ELM_VALUE, REC_CREATE_TIME FROM TMMSM81AL WHERE MAT_CODE = A.MAT_CODE AND ELM_NAME = 'Mo' and QUALITY_BATCH_NO =(SELECT QUALITY_BATCH_NO from (SELECT QUALITY_BATCH_NO,row_number() over(partition by LOT_NO order by REC_CREATE_TIME desc) as rn  FROM TMMSM81AH t2 WHERE t2.MAT_CODE =A.MAT_CODE AND REMARK_1= 'F' and t2.lot_no=A.lot_no) where rn = 1))) where rn = 1), 0)"
				"  when QUALITY_BATCH_NO != ' ' then nvl((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE = A.MAT_CODE  AND ELM_NAME = 'Mo' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO), 0)"
				" else nvl((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE = A.MAT_CODE  AND ELM_NAME = 'Mo' and QUALITY_BATCH_NO in(SELECT MAX(QUALITY_BATCH_NO) FROM tmmsm81ah	"
				"     WHERE REC_CREATE_TIME in(select max(REC_CREATE_TIME) from tmmsm81ah t2 where  t2.MAT_CODE = A.MAT_CODE))), 0)"
				" END MAT_MO "
				" ,case when lot_no in(select lot_no from TMMSMWQ) then '熔清'"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '1') then '固定1'"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '2') then '固定2'"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '3') then '固定3'"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '4') then '固定4'"
				"  when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '5') then '固定5'"
				"  when lot_no != ' ' and lot_no in (select lot_no from  TMMSM81AH t2 where  t2.LOT_NO =LOT_NO AND REMARK_1 = 'F') then '批次号'"
				"  when QUALITY_BATCH_NO != ' ' and QUALITY_BATCH_NO  in (select QUALITY_BATCH_NO FROM TMMSM81AH t2 WHERE  REMARK_1 = 'F' ) then '质检批复验'"
				"  when QUALITY_BATCH_NO != ' ' and QUALITY_BATCH_NO  in (select QUALITY_BATCH_NO FROM TMMSM81AH t2 WHERE  REMARK_1!= 'F' ) then '质检批'"
				"  else '该物料最近成分'"
				"  END TYPE_DESC "
				" ,NVL((SELECT ELM_VALUE FROM TMMSM81AL t1,TMMSM81AH t2 WHERE t1.QUALITY_BATCH_NO =t2.QUALITY_BATCH_NO AND t2.REMARK_1 = 'Y' and t1.MAT_CODE = A.MAT_CODE AND t1.LOT_NO= A.LOT_NO AND ELM_NAME = 'Mo'), 0) AS YD_MO "
				" ,NVL((SELECT ELM_VALUE FROM TMMSM81AL t1,TMMSM81AH t2 WHERE t1.QUALITY_BATCH_NO =t2.QUALITY_BATCH_NO AND t2.REMARK_1 = 'Y' and t1.MAT_CODE = A.MAT_CODE AND t1.LOT_NO= A.LOT_NO AND ELM_NAME = 'Cr'), 0) AS YD_CR "
				" ,NVL((SELECT ELM_VALUE FROM TMMSM81AL t1,TMMSM81AH t2 WHERE t1.QUALITY_BATCH_NO =t2.QUALITY_BATCH_NO AND t2.REMARK_1 = 'Y' and t1.MAT_CODE = A.MAT_CODE AND t1.LOT_NO= A.LOT_NO AND ELM_NAME = 'Ni'), 0) AS YD_NI "
				" from "
				"  (select  F.heat_no,TD_REMAIN_WT,END_TIME AS LADLE_CLOSE_TIME,TD_NO_1,MOLD_NO1,CAST_DIV_NO  "
				", F.ST_NO ,F.LADLE_ARRIVE_WT-F.LADLE_LEAVE_WT  OUT_STEEL_WT ,LADLE_OPEN_TIME"
				", (CASE WHEN SUBSTR(F.ST_NO, 0, 2) = '1A' OR SUBSTR(F.ST_NO, 0, 2) = '1D' THEN '镍钢' WHEN SUBSTR(F.ST_NO, 0, 2) = '1M' OR SUBSTR(F.ST_NO, 0, 2) = '1F' THEN '铬钢' WHEN SUBSTR(F.ST_NO, 0, 1) = '1' THEN '不锈钢' else '碳钢' end) ST_NO_CLASS"
				"   ,nvl(((SELECT  LISTAGG(DISTINCT DEV_CODE,'') WITHIN GROUP (ORDER BY START_TIME) FROM TMMSMGY06  t2 where  t2.heat_no=F.heat_no)), '') as ROUTELIST  "
				",nvl((select SUM(MAT_ACT_WT) FROM VMMSMCPCL_BB B where B.heat_no = F.heat_no), 0) AS MAT_ACT_WT"
				",nvl((select SUM(RECEIVE_WEIGHT) FROM VMMSMCPCL_BB B where B.heat_no = F.heat_no), 0) AS RAW_WEIGHT"
				",nvl((select MAX(ST_NO_DESC) FROM TQMTS0X C where C.ST_NO = F.ST_NO), ' ') AS ST_NO_DESC"
				", nvl(CASE WHEN H.ELM_001 = -1 THEN 0 ELSE ELM_001 END, 0) C "
				", nvl(CASE WHEN H.ELM_002 = -1 THEN 0 ELSE ELM_002 END, 0) SI "
				", nvl(CASE WHEN H.ELM_003 = -1 THEN 0 ELSE ELM_003 END, 0) MN "
				", nvl(CASE WHEN H.ELM_004 = -1 THEN 0 ELSE ELM_004 END, 0) P  "
				", nvl(CASE WHEN H.ELM_005 = -1 THEN 0 ELSE ELM_005 END, 0) S "
				", nvl(CASE WHEN H.ELM_006 = -1 THEN 0 ELSE ELM_006 END, 0) CR	"
				", nvl(CASE WHEN H.ELM_007 = -1 THEN 0 ELSE ELM_007 END, 0) NI "
				", nvl(CASE WHEN H.ELM_008 = -1 THEN 0 ELSE ELM_008 END, 0) MO "
				", nvl(CASE WHEN H.ELM_009 = -1 THEN 0 ELSE ELM_009 END, 0) CU "
				", nvl(CASE WHEN H.ELM_010 = -1 THEN 0 ELSE ELM_010 END, 0) AI "
				", nvl(CASE WHEN H.ELM_011 = -1 THEN 0 ELSE ELM_011 END, 0) NB "
				", nvl(CASE WHEN H.ELM_012 = -1 THEN 0 ELSE ELM_012 END, 0) V  "
				", nvl(CASE WHEN H.ELM_013 = -1 THEN 0 ELSE ELM_013 END, 0) TI	"
				", nvl(CASE WHEN H.ELM_016 = -1 THEN 0 ELSE ELM_016 END, 0) N	"
				",nvl((SELECT MAX(end_time) end_time FROM(select end_time from tmmsm21 t2 where t2.heat_no = F.heat_no union select end_time from tmmsm27 t2 where  t2.heat_no = F.heat_no)), ' ') TAP_TIME"
				" from tmmsm31  F left join TQMTSB0 H on F.HEAT_NO=H.HEAT_NO"
				" where 1=1"
				" and F.heat_no in (select heat_no from tmmsm21 t2 where st_no != 'DeP' and st_no not like '1%'  and  end_time<=@end_time and end_time>=@begin_time union  select heat_no from tmmsm27 t2 where end_time<=@end_time and end_time>=@begin_time)"
				;
			if (heat_no.Trim() != "")
			{
				sqlstr += " AND F.HEAT_NO  =@heat_no";
			}
			if (c_div.Trim() == "1")  //不锈钢
			{
				sqlstr += " and substr(F.ST_NO,1,1) in ('1','4')";
			}
			if (c_div.Trim() == "2")  //碳钢钢
			{
				sqlstr += " and substr(F.ST_NO,1,1) in ('2','3','5')";
			}
			sqlstr += ") t1 left join ("
				" select heat_no,mat_code,dev_code,QUALITY_BATCH_NO,LOT_NO,sum(DEVO_WT*0.001) DEVO_WT"
				" from tmmsmgy08 A"
				" where 1=1"
				//" and MAT_CODE not in ( SELECT MAT_CODE FROM TMMSM50 WHERE SEND_FLAG = '1')"
				" and exists (select 1 from tmmsm31 t2 where A.heat_no = t2.heat_no) "
				" and heat_no in (select heat_no from tmmsm21 t2 where st_no != 'DeP' and st_no not like '1%'  and  end_time<=@end_time and end_time>=@begin_time union  select heat_no from tmmsm27 t2 where end_time<=@end_time and end_time>=@begin_time)"
				" group by heat_no,mat_code,dev_code,QUALITY_BATCH_NO,LOT_NO "
				" ) A on t1.heat_no=A.heat_no"
				" left join tqmtscb08_dr E ON E.MAT_CODE_DR = A.MAT_CODE  "
				" WHERE 1=1"
				;

			if (mat_code.Trim() != "")
			{
				sqlstr += " AND nvl(A.MAT_CODE,' ') =@mat_code";
			}
			if (cast_div_no.Trim() != "")
			{
				sqlstr += " AND nvl(CAST_DIV_NO,' ') =@cast_div_no";
			}
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("mat_code", mat_code);
			cmd_inq.Parameters.Set("cast_div_no", cast_div_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
			cmd_inq.Close();
			//返回分页总数量信息 
			bcls_ret->Tables.Add("PAGEINFO");	//增加块
			bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
			bcls_ret->Tables["PAGEINFO"].Rows.Add();
			bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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