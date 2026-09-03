/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 铬镍收得率明细表
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmzxhlh_ins)

int f_mmsmzxhlh_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString end_time = " ";
	CString time_stamps = " ";
	CString user_idqi = " ";
	CString start_time = " ";
	CString user_id = " ";
	CModel tmmsmzxhbb_lh("TMMSMZXHBB_LH");

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString origin_sys_code = "";
	CString heat_no = "";
	CDecimal prod_out_wt = 0;

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);

	try
	{
		tmmsmzxhbb_lh.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().TrimOrBlank().ToUpper();
		

		time_stamps = datetime;
		user_id = s.userid;
		tmmsmzxhbb_lh["TIME_STAMPS"] = time_stamps;
		tmmsmzxhbb_lh["USER_ID"] = user_id;

		if (tmmsmzxhbb_lh.QueryCount("USER_ID,TIME_STAMPS") > 0)
		{
			strcpy(s.msg, "用户[" + tmmsmzxhbb_lh["USER_ID"].ToString() + "]、执行时间[" + tmmsmzxhbb_lh["TIME_STAMPS"].ToString() + "]已经操作过");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
		
		if (tmmsmzxhbb_lh["AOD0_S"].ToString().Trim() == "" || tmmsmzxhbb_lh["AOD1_S"].ToString().Trim() == "" || tmmsmzxhbb_lh["AOD2_S"].ToString().Trim() == "" || tmmsmzxhbb_lh["AOD6_S"].ToString().Trim() == ""
			|| tmmsmzxhbb_lh["BOF0_S"].ToString().Trim() == "" || tmmsmzxhbb_lh["BOF1_S"].ToString().Trim() == "" || tmmsmzxhbb_lh["BOF2_S"].ToString().Trim() == "" || tmmsmzxhbb_lh["BOF9_S"].ToString().Trim() == "")
		{
			strcpy(s.msg, "开始炉号不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//当时最大的炉号
		if (tmmsmzxhbb_lh["AOD0_E"].ToString().Trim() == "" || tmmsmzxhbb_lh["AOD1_E"].ToString().Trim() == "" || tmmsmzxhbb_lh["AOD2_E"].ToString().Trim() == "" || tmmsmzxhbb_lh["AOD6_E"].ToString().Trim() == "")
		{
			sqlstr = " select max(case when heat_no like 'A0%' then heat_no else 'A04000001' end)"
				",max(case when heat_no like 'A1%' then heat_no else 'A14000001' end)"
				",max(case when heat_no like 'A2%' then heat_no else 'A24000001' end)"
				",max(case when heat_no like 'A6%' then heat_no else 'A64000001' end)"
				" from tmmsm27"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				if (tmmsmzxhbb_lh["AOD0_E"].ToString().Trim() == "")
				{
					tmmsmzxhbb_lh["AOD0_E"] = cmd_inq.GetString(1);
				}
				if (tmmsmzxhbb_lh["AOD1_E"].ToString().Trim() == "")
				{
					tmmsmzxhbb_lh["AOD1_E"] = cmd_inq.GetString(2);
				}
				if (tmmsmzxhbb_lh["AOD2_E"].ToString().Trim() == "")
				{
					tmmsmzxhbb_lh["AOD2_E"] = cmd_inq.GetString(3);
				}
				if (tmmsmzxhbb_lh["AOD6_E"].ToString().Trim() == "")
				{
					tmmsmzxhbb_lh["AOD6_E"] = cmd_inq.GetString(4);
				}
			}
			cmd_inq.Close();
		}

		//当时最大的炉号
		if (tmmsmzxhbb_lh["BOF0_E"].ToString().Trim() == "" || tmmsmzxhbb_lh["BOF1_E"].ToString().Trim() == "" || tmmsmzxhbb_lh["BOF2_E"].ToString().Trim() == "" || tmmsmzxhbb_lh["BOF9_E"].ToString().Trim() == "")
		{
			sqlstr = " select max(case when heat_no like 'B0%' then heat_no else 'B04000001' end)"
				",max(case when heat_no like 'B1%' then heat_no else 'B14000001' end)"
				",max(case when heat_no like 'B2%' then heat_no else 'B24000001' end)"
				",max(case when heat_no like 'B9%' then heat_no else 'B94000001' end)"
				" from tmmsm21"
				" where 1=1 and st_no != 'DeP' and st_no not like '1%' "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				if (tmmsmzxhbb_lh["BOF0_E"].ToString().Trim() == "")
				{
					tmmsmzxhbb_lh["BOF0_E"] = cmd_inq.GetString(1);
				}
				if (tmmsmzxhbb_lh["BOF1_E"].ToString().Trim() == "")
				{
					tmmsmzxhbb_lh["BOF1_E"] = cmd_inq.GetString(2);
				}
				if (tmmsmzxhbb_lh["BOF2_E"].ToString().Trim() == "")
				{
					tmmsmzxhbb_lh["BOF2_E"] = cmd_inq.GetString(3);
				}
				if (tmmsmzxhbb_lh["BOF9_E"].ToString().Trim() == "")
				{
					tmmsmzxhbb_lh["BOF9_E"] = cmd_inq.GetString(4);
				}
			}
			cmd_inq.Close();
		}

		tmmsmzxhbb_lh["REC_CREATOR"] = s.userid;
		tmmsmzxhbb_lh["REC_CREATE_TIME"] = datetime;
		tmmsmzxhbb_lh["CHECK_FLAG"] = "0"; 		
		tmmsmzxhbb_lh.TrimOrBlank();		
		tmmsmzxhbb_lh.Insert();	

		//执行铬镍计算操作
		sqlstr = " delete from tmmsmzxhbb t1  "
			" where 1=1"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr_temp = " and ("
			"   (HEAT_NO <= '" + tmmsmzxhbb_lh["AOD0_E"].ToString() + "' AND HEAT_NO >= '" + tmmsmzxhbb_lh["AOD0_S"].ToString() + "') "
			" OR( HEAT_NO <= '" + tmmsmzxhbb_lh["AOD1_E"].ToString() + "' AND HEAT_NO >= '" + tmmsmzxhbb_lh["AOD1_S"].ToString() + "' ) "
			" OR( HEAT_NO <= '" + tmmsmzxhbb_lh["AOD2_E"].ToString() + "' AND HEAT_NO >= '" + tmmsmzxhbb_lh["AOD2_S"].ToString() + "')"
			" OR( HEAT_NO <= '" + tmmsmzxhbb_lh["AOD6_E"].ToString() + "' AND HEAT_NO >= '" + tmmsmzxhbb_lh["AOD6_S"].ToString() + "') "
			" OR( HEAT_NO <= '" + tmmsmzxhbb_lh["BOF0_E"].ToString() + "' AND HEAT_NO >= '" + tmmsmzxhbb_lh["BOF0_S"].ToString() + "')"
			" OR( HEAT_NO <= '" + tmmsmzxhbb_lh["BOF1_E"].ToString() + "' AND HEAT_NO >= '" + tmmsmzxhbb_lh["BOF1_S"].ToString() + "') "
			" OR( HEAT_NO <= '" + tmmsmzxhbb_lh["BOF2_E"].ToString() + "' AND HEAT_NO >= '" + tmmsmzxhbb_lh["BOF2_S"].ToString() + "') "
			" OR( HEAT_NO <= '" + tmmsmzxhbb_lh["BOF9_E"].ToString() + "' AND HEAT_NO >= '" + tmmsmzxhbb_lh["BOF9_S"].ToString() + "')"

			;
		if (tmmsmzxhbb_lh["HEAT_IN"].ToString().Trim() != "")
		{
			tmmsmzxhbb_lh["HEAT_IN"] = tmmsmzxhbb_lh["HEAT_IN"].ToString().Replace(",", "','");
			sqlstr_temp += " OR ( heat_no in ('" + tmmsmzxhbb_lh["HEAT_IN"].ToString() + "') )";
		}
		sqlstr_temp += " ) ";
		if (tmmsmzxhbb_lh["HEAT_OUT"].ToString().Trim() != "")
		{
			tmmsmzxhbb_lh["HEAT_OUT"] = tmmsmzxhbb_lh["HEAT_OUT"].ToString().Replace(",", "','");
			sqlstr_temp += " and heat_no not in ('" + tmmsmzxhbb_lh["HEAT_OUT"].ToString() + "')";
		}

		//插入主数据
		sqlstr = " insert into tmmsmzxhbb (REC_CREATOR,REC_CREATE_TIME,time_stamps,USER_ID,HEAT_NO,PROC_NO,DEV_CODE,MAT_CODE,MAT_NAME,LOT_NO,WEIGH_NO,QUALITY_BATCH_NO,ST_NO,DEVO_WT,TYPE_DESC,ST_NO_SMALL_CLASS1)"
			" select @rec_creator,@rec_create_time,@time_stamps,@user_id,heat_no,SM_PLAN_NOL2,DEV_CODE,MAT_CODE,MAT_NAME,LOT_NO,WEIGH_NO,QUALITY_BATCH_NO,ST_NO,sum(OUT_STOCK_WT/1000),'计量单成分'"
			",(CASE WHEN SUBSTR(ST_NO, 0, 2) in('1A', '1D') THEN '镍钢' WHEN SUBSTR(ST_NO, 0, 2) in ( '1M' ,'1F') THEN '铬钢' 	WHEN SUBSTR(ST_NO, 0, 1) = '1' and SUBSTR(ST_NO, 0, 2) not in ( '1A' ,'1D','1M' ,'1F') THEN '不锈钢' else '碳钢' end)"
			" from tmmsm56 t1"
			" where   1=1"
			" and mat_code IN (SELECT MAT_CODE FROM TMMSM50 WHERE SEND_FLAG != '1') "
			+ sqlstr_temp +
			" group by  HEAT_NO,SM_PLAN_NOL2,DEV_CODE,MAT_CODE,MAT_NAME,LOT_NO,WEIGH_NO,QUALITY_BATCH_NO,ST_NO"
			;
		Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", datetime);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新钢种
		sqlstr = " update tmmsmzxhbb t1 set (ST_NO_DESC,ST_NO_BIG_CLASS) = ( select SG_GRADE_1,ST_NO_BIG_CLASS from  TQMTS0X  t2 where t1.st_no = t2.st_no) "
			" where 1=1"
			" and exists (select 1 from TQMTS0X  t2 where t1.st_no = t2.st_no)"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//物料类别
		sqlstr = " update tmmsmzxhbb t1 set TYPE_CODE1 = ( select decode(TYPE_DL, '10', '辅料', '20', '合金', '30', '废钢', '70', '生铁', '80', '铁水', '90', '自循环废钢',' ') from  ZJ_JISHUKE_MAT  t2 where t1.MAT_CODE = t2.MAT_ID) "
			" where 1=1"
			" and exists (select 1 from ZJ_JISHUKE_MAT  t2 where t1.MAT_CODE = t2.MAT_ID)"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新钢水
		//更新连铸炉次班组 ，钢水
		sqlstr = " update tmmsmzxhbb t1 set (TAP_END_TIME,PROD_OUT_WT,SHIFT_GROUP,SHIFT_NO,ORIGIN_SYS_CODE) = (select LADLE_ARRIVE_TIME,LADLE_ARRIVE_WT-LADLE_LEAVE_WT,PROD_SHIFT_GROUP,PROD_SHIFT_NO,'CCM' from tmmsm31 t2 where t1.heat_no=t2.heat_no)"
			" where 1=1"
			" and ORIGIN_SYS_CODE =' '"
			" and exists (select 1 from tmmsm31 t2 where t1.heat_no=t2.heat_no)"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新工艺路径
		sqlstr = " update tmmsmzxhbb t1 set ROUTELIST = (SELECT  LISTAGG(DISTINCT DEV_CODE,'/') WITHIN GROUP (ORDER BY START_TIME) FROM TMMSMGY06  t2 where t2.HANDLE_DIV=' ' and t1.heat_no=t2.heat_no)"
			" where 1=1"
			" and exists (select 1 from TMMSMGY06 t2 where t1.heat_no=t2.heat_no)"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新钢水量	1、一级 2、连铸 3、AOD  4、连铸前最后一个精炼
		sqlstr = " update tmmsmzxhbb t1 set PROD_OUT_WT = (select STEEL_NET_WEIGHT from VW_CCM_STEEL_WT t2 where t1.heat_no=t2.HEAT_NAME and rownum=1) ,ORIGIN_SYS_CODE='L1'"
			" where 1=1"
			" and exists (select 1 from VW_CCM_STEEL_WT t2 where t1.heat_no=t2.HEAT_NAME)"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		sqlstr = " update tmmsmzxhbb t1 set PROD_OUT_WT = (select ACTRESULT from tmmsm27 t2 where t1.heat_no=t2.l2_proc_no) ,ORIGIN_SYS_CODE='AOD'"
			" where 1=1"
			" and PROD_OUT_WT <= 0 "
			" and heat_no like 'A%'"
			" and exists (select 1 from tmmsm27 t2 where t1.heat_no=t2.l2_proc_no)"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//如果是转炉而且没有钢水的按消耗量按连铸前一个个去找
		sqlstr = " select distinct heat_no"
			" from tmmsmzxhbb"
			" where 1=1"
			" and PROD_OUT_WT <= 0 "
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			heat_no = cmd_inq.GetString(1);
			prod_out_wt = 0;
			origin_sys_code = " ";
			sqlstr = "select dev_code,l2_proc_no"
				" from tmmsmgy06"
				" where 1=1"
				" and dev_code not like 'C%'"
				" and heat_no = @heat_no"
				" order by START_TIME desc"
				;
			cmd_inq1.SetCommandText(sqlstr);
			cmd_inq1.Parameters.Set("heat_no", heat_no);
			cmd_inq1.ExecuteReader();
			while (cmd_inq1.Read())
			{

				if (cmd_inq1.GetString(1).SubstringNE(0, 1) == "F") // LF
				{
					sqlstr = " SELECT MOLTIRON_WT FROM TMMSM24 WHERE l2_proc_no = @heat_no";
					cmd_inq2.Parameters.Set("heat_no", heat_no);
					cmd_inq2.SetCommandText(sqlstr);
					cmd_inq2.ExecuteReader();
					if (cmd_inq2.Read())
					{
						prod_out_wt = cmd_inq2.GetDecimal(1);
						origin_sys_code = "LF";
					}
					cmd_inq2.Close();
				}
				else if (cmd_inq1.GetString(1).SubstringNE(0, 1) == "S") // LTS
				{
					sqlstr = " SELECT STEEL_WT FROM TMMSM26 WHERE l2_proc_no = @heat_no";
					cmd_inq2.Parameters.Set("heat_no", heat_no);
					cmd_inq2.SetCommandText(sqlstr);
					cmd_inq2.ExecuteReader();
					if (cmd_inq2.Read())
					{
						prod_out_wt = cmd_inq2.GetDecimal(1);
						origin_sys_code = "LTS";
					}
					cmd_inq2.Close();

				}
				else if (cmd_inq1.GetString(1).SubstringNE(0, 1) == "V") // VOD
				{
					sqlstr = " SELECT ACTRESULT FROM TMMSM25 WHERE l2_proc_no = @heat_no";
					cmd_inq2.Parameters.Set("heat_no", heat_no);
					cmd_inq2.SetCommandText(sqlstr);
					cmd_inq2.ExecuteReader();
					if (cmd_inq2.Read())
					{
						prod_out_wt = cmd_inq2.GetDecimal(1);
						origin_sys_code = "VOD";
					}
					cmd_inq2.Close();

				}
				else if (cmd_inq1.GetString(1).SubstringNE(0, 1) == "R") // RH
				{
					sqlstr = " SELECT ACTRESULT FROM TMMSM23 WHERE l2_proc_no = @heat_no";
					cmd_inq2.Parameters.Set("heat_no", heat_no);
					cmd_inq2.SetCommandText(sqlstr);
					cmd_inq2.ExecuteReader();
					if (cmd_inq2.Read())
					{
						prod_out_wt = cmd_inq2.GetDecimal(1);
						origin_sys_code = "RH";
					}
					cmd_inq2.Close();
				}
				else // 都没有取转炉重量
				{
					sqlstr = " SELECT ACTRESULT FROM TMMSM21 WHERE l2_proc_no = @heat_no";
					cmd_inq2.Parameters.Set("heat_no", heat_no);
					cmd_inq2.SetCommandText(sqlstr);
					cmd_inq2.ExecuteReader();
					if (cmd_inq2.Read())
					{
						prod_out_wt = cmd_inq2.GetDecimal(1);
						origin_sys_code = "BOF";
					}
					cmd_inq2.Close();
				}

				if (prod_out_wt > 0)
				{
					break;
				}
			}
			cmd_inq1.Close();

			//更新值
			if (prod_out_wt > 0)
			{
				sqlstr = " update tmmsmzxhbb t1 set PROD_OUT_WT =@prod_out_wt ,ORIGIN_SYS_CODE=@origin_sys_code"
					" where 1=1"
					" and heat_no = @heat_no"
					;
				cmd_inq1.SetCommandText(sqlstr);
				cmd_inq1.Parameters.Set("heat_no", heat_no);
				cmd_inq1.Parameters.Set("prod_out_wt", prod_out_wt);
				cmd_inq1.Parameters.Set("origin_sys_code", origin_sys_code);
				cmd_inq1.ExecuteNonQuery();
				cmd_inq1.Close();
			}
		}
		cmd_inq.Close();


		//更新成分 优先级：
		//固定表成分1
		//熔清成分 TMMSMWQ
		//复验补录 		
		//计量单成分
		//该物料最近成分 
		//固定表（ZJ_MAT_ELEMENT类别：1.按表内成分填写，2用成分乘以表中系数，3只用表内Cr成分，4只用表中Ni成分）

		sqlstr = " update tmmsmzxhbb t1 set (QUALITY_BATCH_NO,CR_VALUE,NI_VALUE,MO_VALUE) = (select 'ZJ_MAT_ELEMENT',CR,NI,MO from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID  and t2.TYPE='1') ,TYPE_DESC='固定类型1'"
			" where 1=1"
			" and TYPE_DESC ='计量单成分'"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='1')"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//熔清
		//sqlstr = " update tmmsmzxhbb t1 set  (QUALITY_BATCH_NO,CR_VALUE,NI_VALUE,MO_VALUE) = (select LOT_NO,nvl(CR_VALUE,0),nvl(NI_VALUE,0),nvl(MO_VALUE,0) from TMMSMWQ t2 where t1.LOT_NO=t2.LOT_NO) ,TYPE_DESC='熔清成分'"
			//" where 1=1"
			//" and exists (select 1 from TMMSMWQ t2 where t1.LOT_NO=t2.LOT_NO)"
			//" and TYPE_DESC ='计量单成分'"
			//" and  time_stamps = @time_stamps"
			//" and  user_id = @user_id"
			//;
		//熔清20251031
		sqlstr = " update tmmsmzxhbb t1 set (QUALITY_BATCH_NO, CR_VALUE, NI_VALUE, MO_VALUE) =(SELECT LOT_NO, CR_VALUE, NI_VALUE, MO_VALUE FROM (SELECT LOT_NO,nvl(CR_VALUE, 0) as CR_VALUE,nvl(NI_VALUE, 0) as NI_VALUE,nvl(MO_VALUE, 0) as MO_VALUE,row_number() over (partition by LOT_NO order by REC_CREATE_TIME desc) as rn FROM TMMSMWQ t2 WHERE t1.LOT_NO = t2.LOT_NO) WHERE rn = 1), TYPE_DESC = '熔清成分'"
			" where 1=1"
			" and exists (select 1 from TMMSMWQ t2 where t1.LOT_NO=t2.LOT_NO)"
			" and TYPE_DESC ='计量单成分'"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新复验	
		sqlstr = " update tmmsmzxhbb t1 set QUALITY_BATCH_NO= (SELECT  max(QUALITY_BATCH_NO) FROM TMMSM81AH t2 WHERE  t2.REMARK_1 = 'F' AND t2.MAT_CODE=t1.MAT_CODE and  t2.LOT_NO=t1.LOT_NO )"
			" ,TYPE_DESC='复验成分'"
			" where 1=1"
			" and exists (select 1 from  TMMSM81AH t2 where  t2.REMARK_1 = 'F' AND t1.MAT_CODE=t2.MAT_CODE and  t1.LOT_NO=t2.LOT_NO )"
			" and TYPE_DESC ='计量单成分'"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsmzxhbb t1 set QUALITY_BATCH_NO = (SELECT MAX(QUALITY_BATCH_NO) FROM tmmsm81ah"
			" WHERE REC_CREATE_TIME in (select max(REC_CREATE_TIME) from tmmsm81ah t2 where  t2.MAT_CODE=t1.MAT_CODE )) ,TYPE_DESC='最近质检批'"
			" where 1=1"
			" and QUALITY_BATCH_NO =' ' "
			" and exists (select 1 from tmmsm81ah t2 where t1.MAT_CODE=t2.MAT_CODE) "
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"

			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		//更新物料最近成分
		sqlstr = " update tmmsmzxhbb t1 set (QUALITY_BATCH_NO,CR_VALUE,NI_VALUE,MO_VALUE)= (SELECT QUALITY_BATCH_NO ,max(nvl(Cr,0)),max(nvl(Ni,0)), max(nvl(Mo,0))  FROM ("
			" SELECT  * FROM ( SELECT QUALITY_BATCH_NO ,ELM_VALUE,ELM_NAME FROM  TMMSM81AL )"
			" PIVOT ( SUM(ELM_VALUE) FOR ELM_NAME IN ( 'Cr' AS Cr ,'Ni' AS Ni, 'Mo' AS Mo))) t2  where t1.QUALITY_BATCH_NO=t2.QUALITY_BATCH_NO GROUP BY  QUALITY_BATCH_NO)"
			" where 1=1"
			" and TYPE_DESC not in ('固定类型1','熔清成分') "
			" and exists (select 1 from  TMMSM81AL t2 where t1.QUALITY_BATCH_NO=t2.QUALITY_BATCH_NO)"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();



		sqlstr = " update tmmsmzxhbb t1 set (CR_VALUE) = (select CR from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID) ,TYPE_DESC='固定类型3'"
			" where 1=1"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='3')"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsmzxhbb t1 set (NI_VALUE) = (select NI from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID) ,TYPE_DESC='固定类型4'"
			" where 1=1"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='4')"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsmzxhbb t1 set (MO_VALUE) = (select MO from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID) ,TYPE_DESC='固定类型5'"
			" where 1=1"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='5')"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsmzxhbb t1 set (CR_VALUE,NI_VALUE) = (select CR*t1.CR_VALUE,NI*t1.NI_VALUE from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID) ,TYPE_DESC='固定类型2'"
			" where 1=1"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='2')"
			" and  time_stamps = @time_stamps"
			" and  user_id = @user_id"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();	
		
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
