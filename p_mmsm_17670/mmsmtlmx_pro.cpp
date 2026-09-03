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
BM2F_ENTERACE(mmsmtlmx_pro)

int f_mmsmtlmx_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int count = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	
	CString end_time = " ";
	CString begin_time = " ";
	CString heat_no = "";
	CString mat_code = "";
	CDecimal cd_count = 0; 
	CString gd_flag = "0";
	CString c_div = "";


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	try
	{

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
	
		if (begin_time.Trim() == "" || end_time.Trim() == "")
		{
			strcpy(s.msg, "开始时间或结束时间不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (gd_flag != "1")
		{


			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " select t1.heat_no AS 炉号,A.dev_code AS 工序,ST_NO AS 牌号,A.mat_code AS 物料代码 "
					",NVL((SELECT MAT_NAME FROM TMMSM50 T WHERE T.MAT_CODE = A.MAT_CODE ),' ')  AS 物料名称 "
					",A.DEVO_WT AS 投料重量,MAT_ACT_WT AS 合格重量 "
					",nvl( (SELECT MAX(end_time) end_time FROM (select end_time from tmmsm21 t2 where t2.heat_no =t1.heat_no union select end_time from tmmsm27 t2 where  t2.heat_no =t1.heat_no  )),' ')  AS 出钢时间"  //出钢时间
					",ST_NO_CLASS AS 钢种分类 "
					",nvl(E.MAT_TYPE_DESC,' ')   AS 物料类型 "
					",ST_NO_DESC AS 钢种分类1,LADLE_OPEN_TIME AS 开浇时间, LADLE_CLOSE_TIME AS 关包时间"
					",nvl((select sum(CUT_SCRAP_WT) from tmmsmfp t3 where  t3.heat_no=t1.heat_no ),0) AS 切头切尾重量"
					", TD_REMAIN_WT AS 中包残钢,RAW_WEIGHT AS 收货重量,OUT_STEEL_WT AS 出钢重量 "
					",C,SI,MN,P,S,CR,NI,MO,CU,AI,NB,V,TI,N "
					" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='C' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_C"
					" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='Si' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_SI"
					" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='Mn' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_MN "
					" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='P' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_P"
					" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='S' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_S"
					" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='Cr' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_CR"
					" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='Ni' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_NI"
					" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='Mo' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_MO" 
					", nvl((select sum(mat_act_wt) from hmmsm01 t3 where t3.complex_decide_code = '9' and t3.heat_no = t1.heat_no), 0) AS 系统废钢 "
					",MOLD_NO1 AS 结晶器号,TD_NO_1 AS 中包号,' ' AS 连铸机流号,' ' AS 中包序号  "
					", CAST_DIV_NO AS 连浇次序, ROUTELIST AS 工序路线  "
					",A.QUALITY_BATCH_NO AS 质检组批号 ,A.LOT_NO AS 批次号 "
					" from "
					"  (select  F.heat_no,TD_REMAIN_WT,LADLE_ARRIVE_TIME,END_TIME AS LADLE_CLOSE_TIME,TD_NO_1,MOLD_NO1,CAST_DIV_NO  "
					", F.ST_NO ,F.LADLE_ARRIVE_WT-F.LADLE_LEAVE_WT  OUT_STEEL_WT ,LADLE_OPEN_TIME"
					", (CASE WHEN SUBSTR(F.ST_NO, 0, 2) = '1A' OR SUBSTR(F.ST_NO, 0, 2) = '1D' THEN '镍钢' WHEN SUBSTR(F.ST_NO, 0, 2) = '1M' OR SUBSTR(F.ST_NO, 0, 2) = '1F' THEN '铬钢' WHEN SUBSTR(F.ST_NO, 0, 1) = '1' THEN '不锈钢' else '碳钢' end) ST_NO_CLASS"
					"  ,nvl(((SELECT  LISTAGG(DISTINCT DEV_CODE,'') WITHIN GROUP (ORDER BY START_TIME) FROM TMMSMGY06  t2 where  t2.heat_no=F.heat_no)), '') as ROUTELIST  "
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
					" from tmmsm31  F left join TQMTSB0 H on F.HEAT_NO=H.HEAT_NO"
					" where 1=1"
					" and F.heat_no in (select heat_no from tmmsm21  where st_no != 'DeP' and st_no not like '1%' and  END_TIME <=  @end_time  and END_TIME >=  @begin_time union select heat_no from tmmsm27  where  END_TIME <=  @end_time  and END_TIME >=  @begin_time)"
					; 					
				if (heat_no.Trim() != "")
				{
					sqlstr += " AND F.HEAT_NO  LIKE @heat_no||'%'";
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
					" and heat_no in (select heat_no from tmmsm21 t2 where st_no != 'DeP' and st_no not like '1%' and END_TIME <=  @end_time  and END_TIME >=  @begin_time union select heat_no from tmmsm27 t2 where  END_TIME <=  @end_time  and END_TIME >=  @begin_time)"
					;
				if (heat_no.Trim() != "")
				{
					sqlstr += " AND HEAT_NO  LIKE @heat_no||'%'";
				}
				if (mat_code.Trim() != "")
				{
					sqlstr += " AND MAT_CODE =@mat_code";
				}
				sqlstr += " group by heat_no,mat_code,dev_code,QUALITY_BATCH_NO,LOT_NO "
					" ) A on t1.heat_no=A.heat_no"
					" left join tqmtscb08_dr E ON E.MAT_CODE_DR = A.MAT_CODE "
					" where 1=1"
					;
				if (mat_code.Trim() != "")
				{
					sqlstr += " AND nvl(A.MAT_CODE,' ') =@mat_code";
				}
				break;
			}

			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("mat_code", mat_code);


			cmd_inq.SetCommandText(sqlstr);
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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
					cmd_inq1.SetCommandText(sqlstr);
					cmd_inq1.Parameters.Clear();
					cmd_inq1.Parameters.Set("tmmsm81al.MAT_CODE", bcls_ret->Tables[0].Rows[i]["物料代码"].ToString().Trim());

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
		}
		else
		{
			sqlstr = " select t1.heat_no AS 炉号,A.dev_code AS 工序,ST_NO AS 牌号,A.mat_code AS 物料代码 "
				",NVL((SELECT MAT_NAME FROM TMMSM50 T WHERE T.MAT_CODE = A.MAT_CODE ),' ')  AS 物料名称 "
				",A.DEVO_WT AS 投料重量,MAT_ACT_WT AS 合格重量 "
				",nvl( (SELECT MAX(end_time) end_time FROM (select end_time from tmmsm21 t2 where t2.heat_no =t1.heat_no union select end_time from tmmsm27 t2 where  t2.heat_no =t1.heat_no  )),' ')  AS 出钢时间"  //出钢时间
				",ST_NO_CLASS AS 钢种分类 "
				",nvl(E.MAT_TYPE_DESC,' ')    AS 物料类型 "
				",ST_NO_DESC AS 钢种分类1,LADLE_OPEN_TIME AS 开浇时间, LADLE_CLOSE_TIME AS 关包时间"
				",nvl((select sum(CUT_SCRAP_WT) from tmmsmfp t3 where  t3.heat_no=t1.heat_no ),0) AS 切头切尾重量"
				", TD_REMAIN_WT AS 中包残钢,RAW_WEIGHT AS 收货重量,OUT_STEEL_WT AS 出钢重量 "
				",C,SI,MN,P,S,CR,NI,MO,CU,AI,NB,V,TI,N "
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='C' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_C"
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='Si' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_SI"
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='Mn' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_MN "
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='P' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_P"
				" ,NVL((SELECT ELM_VALUE  FROM TMMSM81AL  WHERE  MAT_CODE=A.MAT_CODE  AND ELM_NAME='S' and QUALITY_BATCH_NO = A.QUALITY_BATCH_NO),0) MAT_S"
				", case when lot_no in(select lot_no from TMMSMWQ) then round((select max(cr_VALUE) from  TMMSMWQ where lot_no = A.lot_no)*nvl((select INCLUDE_CR from TQMTSCB11_DR where mat_code = A.MAT_CODE),0),4)"
				"    when mat_code in(select MAT_ID from ZJ_MAT_ELEMENT where TYPE = '3') then(select cr from  ZJ_MAT_ELEMENT where TYPE = '3' and MAT_ID = A.MAT_CODE)"
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
				", nvl((select sum(mat_act_wt) from hmmsm01 t3 where t3.complex_decide_code = '9' and t3.heat_no = t1.heat_no), 0) AS 系统废钢 "
				",MOLD_NO1 AS 结晶器号,TD_NO_1 AS 中包号,' ' AS 连铸机流号,' ' AS 中包序号  "
				", CAST_DIV_NO AS 连浇次序, ROUTELIST AS 工序路线  "
				",A.QUALITY_BATCH_NO AS 质检组批号 ,A.LOT_NO AS 批次号 "
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
				"  (select  F.heat_no,TD_REMAIN_WT,LADLE_ARRIVE_TIME,END_TIME AS LADLE_CLOSE_TIME,TD_NO_1,MOLD_NO1,CAST_DIV_NO  "
				", F.ST_NO ,F.LADLE_ARRIVE_WT-F.LADLE_LEAVE_WT  OUT_STEEL_WT ,LADLE_OPEN_TIME"
				", (CASE WHEN SUBSTR(F.ST_NO, 0, 2) = '1A' OR SUBSTR(F.ST_NO, 0, 2) = '1D' THEN '镍钢' WHEN SUBSTR(F.ST_NO, 0, 2) = '1M' OR SUBSTR(F.ST_NO, 0, 2) = '1F' THEN '铬钢' WHEN SUBSTR(F.ST_NO, 0, 1) = '1' THEN '不锈钢' else '碳钢' end) ST_NO_CLASS"
				"  ,nvl(((SELECT  LISTAGG(DISTINCT DEV_CODE,'') WITHIN GROUP (ORDER BY START_TIME) FROM TMMSMGY06  t2 where  t2.heat_no=F.heat_no)), '') as ROUTELIST  "
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
				" from tmmsm31  F left join TQMTSB0 H on F.HEAT_NO=H.HEAT_NO"
				" where 1=1"
				" and F.heat_no in (select heat_no from tmmsm21  where st_no != 'DeP' and st_no not like '1%' and  END_TIME <=  @end_time  and END_TIME >=  @begin_time union select heat_no from tmmsm27  where  END_TIME <=  @end_time  and END_TIME >=  @begin_time)"
				; 			
			if (heat_no.Trim() != "")
			{
				sqlstr += " AND F.HEAT_NO  LIKE @heat_no||'%'";
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
				" and heat_no in (select heat_no from tmmsm21  where st_no != 'DeP' and st_no not like '1%' and END_TIME <=  @end_time  and END_TIME >=  @begin_time union select heat_no from tmmsm27  where  END_TIME <=  @end_time  and END_TIME >=  @begin_time)"
				;			
			if (heat_no.Trim() != "")
			{
				sqlstr += " AND HEAT_NO  LIKE @heat_no||'%'";
			}
			if (mat_code.Trim() != "")
			{
				sqlstr += " AND MAT_CODE =@mat_code";
			}			
			sqlstr += " group by heat_no,mat_code,dev_code,QUALITY_BATCH_NO,LOT_NO "
				" ) A on t1.heat_no=A.heat_no"
				" left join tqmtscb08_dr E ON E.MAT_CODE_DR = A.MAT_CODE "
				" where 1=1"
				;
			if (mat_code.Trim() != "")
			{
				sqlstr += " AND nvl(A.MAT_CODE,' ') =@mat_code";
			}

			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);			
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("mat_code", mat_code);			
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
			

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