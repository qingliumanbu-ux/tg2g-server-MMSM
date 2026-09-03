/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		MFJ
Version:    1.0
Date:		2024-01-18
Description:炉成本核算计算发送L4电文
连铸叉臂重量 = 钢水量 = a
当炉收货量 = A
折算合格量 = a*x
折算废钢回收量 =  a*x*y
现废 = Z
现废炉次折算合格量 = a*x-Z
现废炉次折算废钢回收量 =  a*x*y+z	

备注：1、合格量折算系数 x=(A+B+C+D+E)/(a+b+c+d+e)  同一个浇次和中包
2、废钢回收折算系数y=（头坯重量+尾坯重量+中包残钢）/(ax+bx+cx+dx+ex)
3、现废量：Z
4、若其中一炉存在现废，则当炉折算合格量=折算合格量-Z，折算废钢回收量=折算废钢回收量+Z，例如：中包第1炉出现现废 Z 吨，则折算合格量=a*x-Z,折算废钢回收量a*x*y+Z

20240927 变更
合格量 = A
合格量折算系数 x=(A+B+C+D+E)/(a+b+c+d+e)  同一个浇次和中包
折算合格量 = a*x
废钢回收折算系数y=（头坯重量+尾坯重量+中包残钢+判废）/(a+b+c+d+e)
现废炉次折算废钢回收量 =   a*y

**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_t80r91_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString tcNO = "T80R91";
	
	CString heat_no = " "; 
	CString resume_seq_no = "";
	CString cast_div_no = " ";
	CString td_no_1 = " ";
	CString cast_div_no_1 = " "; 	
	CString cs_tc_no = "";//电文号
	CString stat_date = " ";
	CDecimal mat_act_wt = 0;
	CDecimal steel_wt = 0;
	CDecimal all_mat_wt = 0;
	CDecimal all_steel_wt = 0;
	CDecimal all_cut_scrap_wt = 0; 	
	CDecimal conversion_ok_wt = 0;
	CDecimal xf_wt = 0;
	CDecimal conversion_alloy_wt = 0;
	
	CModel tmmsm0r91("TMMSM0R91");
	CModel tmmsmopr = CModel("TMMSMOPR");//操作记录表
	EPEX epex;
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm36("TMMSM36");

	// 数据库SQL操作字符串
	CString  sqlstr("");
	CString SeqNo = "";
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inqu(conn);
	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{ 
			
			heat_no = bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString();
			
			tmmsm0r91.Reset();
			tmmsm0r91["HEAT_NO"] = heat_no;

			 mat_act_wt = 0;
			 steel_wt = 0;
			 all_mat_wt = 0;
			 all_steel_wt = 0;
			 conversion_ok_wt = 0;
			 xf_wt = 0;
			 conversion_alloy_wt = 0;
			 
			
			Log::Trace("", __FUNCTION__, "HEAT_NO11111[{0}]", heat_no);
			Log::Trace("", __FUNCTION__, "back2[{0}]", resume_seq_no);
			
			if (epex.Initialize("T80R91") < 0)
			{
				sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (epex.SetValue("heat_no", 0, heat_no) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("flag", 0, "I") < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			} 			

			sqlstr = " select dev_code from tmmsm31"
				" where 1=1"
				" and heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm0r91["CC_NO"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();			

			//记账日期
			sqlstr = " select RECV_MAT_TIME,CAST_DIV_NO_1,OUT_STEEL_WT,CC_NO,CAST_DIV_NO,TD_NO_1,stat_date,f_route1 from tmmsmgy05"
				" where 1=1"
				" and heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm0r91["RECV_MAT_TIME"] = cmd_inq.GetString(1);
				tmmsm0r91["CAST_DIV_NO_1"] = cmd_inq.GetString(2);
				tmmsm0r91["STEEL_WT"] = cmd_inq.GetDecimal(3); 
				steel_wt = cmd_inq.GetDecimal(3);
				tmmsm0r91["CAST_DIV_NO"] = cmd_inq.GetString(5);
				tmmsm0r91["TD_NO_1"] = cmd_inq.GetString(6);
				tmmsm0r91["STAT_DATE"] = cmd_inq.GetString(7);
				tmmsm0r91["REMARK_1"] = cmd_inq.GetString(8);
				if (epex.SetValue("posting_date", 0, tmmsm0r91["RECV_MAT_TIME"].ToString().SubstringNE(0,8)) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//工位	浇次炉数	中包号	连铸叉臂重量
				if (epex.SetValue("proc_unit", 0, tmmsm0r91["CC_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (epex.SetValue("cast_num", 0, tmmsm0r91["CAST_DIV_NO"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (epex.SetValue("td_no", 0, tmmsm0r91["TD_NO_1"].ToString()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (epex.SetValue("cc_furcal_arm_wt", 0, tmmsm0r91["STEEL_WT"].ToDecimal()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			cmd_inq.Close();

			//工艺路线
			if (epex.SetValue("remark_1", 0, tmmsm0r91["REMARK_1"].ToString()) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			

			//当炉收货量
			sqlstr =
				" select sum(RECEIVE_WEIGHT) from VMMSMCPCL_BB1"
				" where  heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm0r91["FURNACE_CONSIGN_WT"] = cmd_inq.GetDecimal(1);
				if (epex.SetValue("furnace_consign_wt", 0, tmmsm0r91["FURNACE_CONSIGN_WT"].ToDecimal()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			cmd_inq.Close();



			//计算折算系数
			sqlstr = " select SUM(OUT_STEEL_WT)  "
				" from  tmmsmgy05 t2 "
				" where recv_mat_time!=' ' and t2.cast_div_no_1=@cast_div_no_1 and t2.cast_div_no = @cast_div_no and t2.td_no_1 = @td_no_1 "
				" and stat_date =@stat_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("cast_div_no", tmmsm0r91["CAST_DIV_NO"].ToString());
			cmd_inq.Parameters.Set("cast_div_no_1", tmmsm0r91["CAST_DIV_NO_1"].ToString());
			cmd_inq.Parameters.Set("td_no_1", tmmsm0r91["TD_NO_1"].ToString());
			cmd_inq.Parameters.Set("stat_date", tmmsm0r91["STAT_DATE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				all_steel_wt = cmd_inq.GetDecimal(1);
			}
			cmd_inq.Close();


			sqlstr = " select  sum(mat_act_wt) mat_act_wt"
				" from VMMSMCPCL_BB1 t1 "
				" where exists (select 1 from tmmsmgy05 t2 where recv_mat_time!=' ' and t2.cast_div_no_1 = @cast_div_no_1  and t2.cast_div_no = @cast_div_no and t2.td_no_1 = @td_no_1 and t2.stat_date=@stat_date and  t1.heat_no=t2.heat_no)"					
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("cast_div_no", tmmsm0r91["CAST_DIV_NO"].ToString());
			cmd_inq.Parameters.Set("cast_div_no_1", tmmsm0r91["CAST_DIV_NO_1"].ToString());
			cmd_inq.Parameters.Set("td_no_1", tmmsm0r91["TD_NO_1"].ToString());
			cmd_inq.Parameters.Set("stat_date", tmmsm0r91["STAT_DATE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				all_mat_wt = cmd_inq.GetDecimal(1);
			}
			cmd_inq.Close();


			if (all_steel_wt != 0)
			{
				//合格量折算系数
				tmmsm0r91["OK_CONVERSION_COEFFICIENT"] = (all_mat_wt / all_steel_wt).Round(3);
				if (epex.SetValue("ok_conversion_coefficient", 0, tmmsm0r91["OK_CONVERSION_COEFFICIENT"].ToDecimal()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//折算合格量	
				conversion_ok_wt = (steel_wt*(all_mat_wt / all_steel_wt)).Round(3);
				tmmsm0r91["CONVERSION_OK_WT"] = conversion_ok_wt;
				if (epex.SetValue("conversion_ok_wt", 0, conversion_ok_wt) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			//所有的切废量
			sqlstr = " select sum(CUT_SCRAP_WT) from ("
				" select sum(CUT_SCRAP_WT) CUT_SCRAP_WT"
				" from tmmsmfp t3 "
				" where 1=1 "
				" and heat_no in ( select heat_no from tmmsmgy05 t2 where recv_mat_time != ' ' and t2.cast_div_no_1 = @cast_div_no_1  and t2.cast_div_no = @cast_div_no and t2.td_no_1 = @td_no_1 and stat_date=@stat_date)"
				" union all"
				" select sum(mat_act_wt) CUT_SCRAP_WT "
				" from hmmsm01"
				" where complex_decide_code = '9'"
				" and heat_no in ( select heat_no from tmmsmgy05 t2 where recv_mat_time != ' ' and t2.cast_div_no_1 = @cast_div_no_1  and t2.cast_div_no = @cast_div_no and t2.td_no_1 = @td_no_1 and stat_date=@stat_date)"
				")"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("cast_div_no", tmmsm0r91["CAST_DIV_NO"].ToString());
			cmd_inq.Parameters.Set("cast_div_no_1", tmmsm0r91["CAST_DIV_NO_1"].ToString());
			cmd_inq.Parameters.Set("td_no_1", tmmsm0r91["TD_NO_1"].ToString());
			cmd_inq.Parameters.Set("stat_date", tmmsm0r91["STAT_DATE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				all_cut_scrap_wt = cmd_inq.GetDecimal(1);
			}
			cmd_inq.Close();

			if (all_steel_wt != 0)
			{
				//废钢回收折算系数	
				tmmsm0r91["ALLOY_CONVERSION_COEFFICIENT"] = (all_cut_scrap_wt / all_steel_wt).Round(3);
				if (epex.SetValue("alloy_conversion_coefficient", 0, tmmsm0r91["ALLOY_CONVERSION_COEFFICIENT"].ToDecimal()) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//折算废钢回收量
				conversion_alloy_wt = (steel_wt*(all_cut_scrap_wt / all_steel_wt)).Round(3);
				tmmsm0r91["CONVERSION_ALLOY_WT"] = conversion_alloy_wt;
				if (epex.SetValue("conversion_alloy_wt", 0, conversion_alloy_wt) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				} 				
			} 
			

			////现废（t）
			//sqlstr = " select sum(mat_act_wt) "
			//	" from hmmsm01"
			//	" where complex_decide_code = '9'"
			//	" and heat_no=@heat_no"
			//	;
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("heat_no", heat_no);
			//cmd_inq.ExecuteReader();
			//if (cmd_inq.Read())
			//{
			//	xf_wt = cmd_inq.GetDecimal(1);
			//	if (epex.SetValue("xf_wt", 0, cmd_inq.GetDecimal(1)) < 0)
			//	{
			//		sprintf(s.msg, epex.GetMsg());
			//		throw CApplicationException(-1, s.msg, log.Location);
			//	}
			//}
			//cmd_inq.Close();

				if (epex.SetValue("xf_wt", 0,0) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

			//现废炉次折算合格量
			if (epex.SetValue("xf_conversion_ok_wt", 0, conversion_ok_wt) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//现废炉次折算废钢回收量
			if (epex.SetValue("xf_conversion_alloy_wt", 0, conversion_alloy_wt) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (epex.SetValue("send_time", 0, dateNow) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			sqlstr = "select PROD_SHIFT_GROUP"
				" from tmmsm21 "
				" where heat_no = @heat_no"
				" union "
				"select PROD_SHIFT_GROUP"
				" from tmmsm27 "
				" where heat_no = @heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader(); 
			if (cmd_inq.Read())
			{
				tmmsm0r91["SHIFT_GROUP"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();

			if (epex.SetValue("shift_group", 0, tmmsm0r91["SHIFT_GROUP"].ToString()) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//判断数据是否一致，不一致则发送，否则跳过
			sqlstr = " select count(1)"
					" from tmmsm0r91"
					" where 1=1"
					" and conversion_ok_wt=@conversion_ok_wt"
					" and conversion_alloy_wt=@conversion_alloy_wt"
					" and heat_no=@heat_no"
					;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("conversion_alloy_wt", conversion_alloy_wt);
			cmd_inq.Parameters.Set("conversion_ok_wt", conversion_ok_wt);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			if (cmd_inq.ExecuteScalar() == 0)
			{
				//20241220记录操作日志

				sqlstr = "SELECT  LPAD(TO_CHAR(RESUME_SEQ_NO.NEXTVAL), 8, '0') FROM DUAL ";
				cmd_inqu.SetCommandText(sqlstr);
				cmd_inqu.ExecuteReader();
				if (cmd_inqu.Read())
				{
					SeqNo = cmd_inqu.GetString(1).Trim();
				}
				cmd_inqu.Close();
				resume_seq_no = dateNow+SeqNo;
				tmmsmopr.Reset();
				tmmsmopr["RESUME_SEQ_NO"] = resume_seq_no;
				if (tmmsmopr.QueryCount("RESUME_SEQ_NO") > 0)
				{
					tmmsmopr["REC_REVISOR"] = s.userid;
					tmmsmopr["REC_REVISE_TIME"] = dateNow;
					tmmsmopr["BACKC1"] = "炉号:" + tmmsm0r91["HEAT_NO"].ToString() + ";折算合格量:" + tmmsm0r91["CONVERSION_OK_WT"].ToString() + ";折算废钢回收量:" + tmmsm0r91["CONVERSION_ALLOY_WT"].ToString();
					tmmsmopr.Update("BACKC1", "RESUME_SEQ_NO");
				}
				else
				{
					tmmsmopr["REC_CREATOR"] = s.userid;
					tmmsmopr["REC_CREATE_TIME"] = dateNow;
					tmmsmopr["RESUME_SEQ_NO"] = resume_seq_no;
					tmmsmopr["EVENT_TIME"] = dateNow;
					tmmsmopr["FUNC_ID"] = s.svc_name;
					tmmsmopr["CLIENT_IP"] = s.fore_ip;
					tmmsmopr["EVENT_CODE"] = "SENDZZ";
					tmmsmopr["EVENT_DESC"] = "发送ZZ";
					tmmsmopr["EVENT_NAME"] = "炉成本核算发送";
					tmmsmopr["BACKC1"] = "炉号:" + tmmsm0r91["HEAT_NO"].ToString() + ";折算合格量:" + tmmsm0r91["CONVERSION_OK_WT"].ToString() + ";折算废钢回收量:" + tmmsm0r91["CONVERSION_ALLOY_WT"].ToString();
					tmmsmopr.TrimOrBlank();
					tmmsmopr.Insert();
				}

				tmmsm0r91["C_UPDATESIGN"] = "0";
				tmmsm0r91["RESUME_SEQ_NO"] = resume_seq_no;
				tmmsm0r91["REC_CREATOR"] = s.userid;
				tmmsm0r91["REC_CREATE_TIME"] = dateNow;
				tmmsm0r91.TrimOrBlank();
				tmmsm0r91.Insert();

				
				if (epex.SendTele() < 0)
				{
					strcpy(s.msg, "电文发送失败。");
					Log::Trace("", __FUNCTION__, "电文发送失败[{0}]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

			}
			cmd_inq.Close();
			epex.Uninitialize();
		}




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
