/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-07-04
Description: 板坯切断实绩新增
***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/
//框架公用头文件，勿删
#include "stdafx.h"


//外部函数声明

//匹配命令板坯
int f_mmsm33_pickslab(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//获取重量
int f_mmsm_get_matwt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//连铸铸坯产出处理函数_按件
int f_mmsm3301_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm3301n_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//获取材料号
int f_mmsm_get_matno(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_matno_catch_sm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_mat_no_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_mmsm_get_pono(const CString& heat_no, CString& pono, CString& factory_div, CDbConnection * conn);

int f_mmsm_manage_flag(const CString& slab_type, const CString& ingot_code, CString& manage_flag, CDbConnection * conn);

int f_mmsm33_check(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm33_insert(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm33_insert";                //定义函数英文名称  
	CString FunctionCname = "板坯切断_信息新增";              //定义函数中文名称

	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	/****** 自定义变量 ***** */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;
	CString v_fix_slab_num = "0";
	CString c_sm_unit_no = "";
	CString c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");;          //当前时间
	CString c_heat_confm_flag = "";   //炉次确定标志
	CString c_slab_prod_flag = "";   //板坯产出标记
	CString c_pono = "";              //制造命令号
	CString c_stno = "";              //出钢记号
	CDecimal n_count = 1;             //校验是否已存在该记录
	CString batch_flag = "0";              //批量新增标记
	int mat_seq = 0;
	int cut_times = 1;
	int fetchRowCount = 0;
	CString ifGetMatNo = "";
	CString v_manage_flag = "";
	CString v_ingot_flag = "";
	CString v_pono = "";
	CString v_heat_no = "";
	CString v_slab_no = "";
	CString sqlstr = "";
	EIClass inBlock;          //材料主档信息处理用

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	CModel tmmsm33("TMMSM33");
	CModel hmmsm33("TMMSM33");

	try
	{
		bcls_rec->Tables[0].set_TableName("TMMSM33");
		bcls_rec->Tables["TMMSM33"].Columns.Add(tmmsm33);

		if (!bcls_rec->Tables["TMMSM33"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["TMMSM33"].Columns.Add(DT_STRING, "PROC_DIV");
			for (int i = 0; i < bcls_rec->Tables["TMMSM33"].Rows.get_Count(); i++)
			{
				bcls_rec->Tables["TMMSM33"].Rows[i]["PROC_DIV"] = "N";
			}
		}
		if (!bcls_rec->Tables["TMMSM33"].Columns.Contains("FIN_ST_NO"))
		{
			bcls_rec->Tables["TMMSM33"].Columns.Add(DT_STRING, "FIN_ST_NO");
		}
		if (bcls_rec->Tables.Contains("TPSSM11"))
		{
			v_heat_no = bcls_rec->Tables["TPSSM11"].Rows[0]["HEAT_NO"].ToString();
			//Log::Trace("", "", "v_heat_no={0}", v_heat_no);
		}
		if (bcls_rec->Tables.Contains("TPSSM03"))
		{
			for (int s = 0; s < bcls_rec->Tables["TPSSM03"].Rows.get_Count(); s++)
			{
				v_slab_no = bcls_rec->Tables["TPSSM03"].Rows[0]["SLAB_NO"].ToString();
				//Log::Trace("", "", "v_slab_no={0}", v_slab_no);
			}
		}


		//判断是否是批量新增
		if (bcls_rec->Tables[0].Columns.Contains("BATCH_FLAG"))
		{
			if (bcls_rec->Tables[0].Rows[0]["BATCH_FLAG"].ToString() == "1"){
				batch_flag = "1";
			}
		}
		
		//判断是否是批量新增
		if (bcls_rec->Tables[0].Columns.Contains("CUT_TIMES"))
		{
			n_count = bcls_rec->Tables[0].Rows[0]["CUT_TIMES"].ToDecimal();
		}
		
		Log::Trace("", "", "n_count={0}", n_count);
		Log::Trace("", "", "batch_flag={0}", batch_flag);
		//Log::Info("", __FUNCTION__, "传入参数,tmmsm33.SLAB_TYPE1111111111111[{0}]", tmmsm33["SLAB_TYPE"].ToString());

		if (n_count > 1)
		{
			sqlstr = CString(
				" SELECT COUNT(1) FROM "
				"  (select slab_width  from tpssm03 where pono =@pono group by slab_width ) "
			);
		
		    cmd_inq.SetCommandText(sqlstr);
		    cmd_inq.Parameters.Set("pono", bcls_rec->Tables[0].Rows[0]["PONO"]);
		    cmd_inq.ExecuteReader();
		    if (cmd_inq.Read())
		    {
				if (cmd_inq.GetInt32(1) > 1)
				{
					strcpy(s.sysmsg, "同炉异宽不允许批量切割");
					throw CApplicationException(-1, s.msg, log.Location);
				}
		    }
		    cmd_inq.Close();
		}

		//正常新增
		if (!(batch_flag == "1")){
	
			tmmsm33.MergeFrom(bcls_rec->Tables["TMMSM33"].Rows[0]);
			//Log::Trace("", "", "tmmsm33["MAT_NO"] ={0}", tmmsm33["MAT_NO"].ToString());
			//Log::Trace("", "", "tmmsm33["STATION_ID"] ={0}", tmmsm33["STATION_ID"].ToString());
			//Log::Trace("", "", "tmmsm33["MAT_TUBE"] ={0}", tmmsm33["MAT_TUBE"].ToDecimal());
			Log::Trace("", "", "tmmsm33.PONO_SLAB_1 ={0}", tmmsm33["PONO_SLAB_1"].ToString());
			Log::Trace("", "", "tmmsm33.PONO_SLAB_2 ={0}", tmmsm33["PONO_SLAB_2"].ToString());
			//Log::Trace("", "", "tmmsm33["PONO_SLAB_3"] ={0}", tmmsm33["PONO_SLAB_3"].ToString());
			//Log::Trace("", "", "tmmsm33["PONO_SLAB_4"] ={0}", tmmsm33["PONO_SLAB_4"].ToString());
			Log::Trace("", "", "tmmsm33.SLAB_PLAN_DEST ={0}", tmmsm33["SLAB_PLAN_DEST"].ToString());
			//Log::Trace("", "", "tmmsm33["FIN_ST_NO"] ={0}", tmmsm33["FIN_ST_NO"].ToString());
			//Log::Trace("", "", "tmmsm33["SLAB_LEN"] ={0}", tmmsm33["SLAB_LEN"].ToDecimal());

			if (tmmsm33["MAT_TUBE"].ToDecimal() == 0)
			{
				tmmsm33["MAT_TUBE"] = 1;
			}

#if defined(_SYS_PES)
			//获取此时出钢记号判断结果
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:        // Oracle 数据库
			default:
				sqlstr = CString(
					" SELECT FIN_ST_NO,JUDGE_CODE FROM TQMTS23 "
					"  WHERE HEAT_NO = @heat_no "
					"    AND JUDGE_CODE = '1' "
					);
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tmmsm33["HEAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm33["FIN_ST_NO"] = cmd_inq.GetString(1);
			}
			else
			{
				tmmsm33["FIN_ST_NO"] = " ";
			}
			cmd_inq.Close();
			//Log::Trace("", "", "获取最终出钢记号={0}", tmmsm33["FIN_ST_NO"].ToString());
			bcls_rec->Tables["TMMSM33"].Rows[0]["FIN_ST_NO"] = tmmsm33["FIN_ST_NO"];

#endif


			if (tmmsm33["FACTORY_DIV"].ToString().Trim() == "")
			{
				CString FACTORY_DIV = " ";
				f_mmsm_get_pono(tmmsm33["HEAT_NO"].ToString(), v_pono, FACTORY_DIV, conn);
				bcls_rec->Tables["TMMSM33"].Rows[0]["FACTORY_DIV"] = FACTORY_DIV;
			}
			if (tmmsm33["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm33["PROD_SHIFT_GROUP"].ToString().Trim() == "")
			{
				CString PROD_SHIFT_NO = " ";
				CString PROD_SHIFT_GROUP = " ";
				f_epep_get_shift_group("SM", tmmsm33["SLAB_CUT_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
				//Log::Trace("", __FUNCTION__, "tmmsm33["SLAB_CUT_TIME"] ={0},tmmsm33["PROD_SHIFT_NO"] ={1},tmmsm33["PROD_SHIFT_GROUP"] ={2}", tmmsm33["SLAB_CUT_TIME"].ToString(), tmmsm33["PROD_SHIFT_NO"].ToString(), tmmsm33["PROD_SHIFT_GROUP"].ToString());
				bcls_rec->Tables["TMMSM33"].Rows[0]["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
				bcls_rec->Tables["TMMSM33"].Rows[0]["PROD_SHIFT_GROUP"] = PROD_SHIFT_GROUP;
			}

			bcls_rec->Tables["TMMSM33"].Rows[0]["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			bcls_rec->Tables["TMMSM33"].Rows[0]["REC_CREATOR"] = s.userid;

			//判断按件还是按批
			f_mmsm_manage_flag(tmmsm33["SLAB_TYPE"].ToString(), tmmsm33["INGOT_CODE"].ToString(), v_manage_flag, conn);

			//Log::Trace("", "", "v_manage_flag={0}", v_manage_flag);

			bcls_rec->Tables["TMMSM33"].Rows[0]["MANAGE_FLAG"] = v_manage_flag;

			if (v_manage_flag == "2") //板坯和非板坯中按支管理的
			{
				if (tmmsm33["STRAND_NO"].ToString().Trim() == "")
				{
					strcpy(s.sysmsg, "流号不能为空");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//n_count = tmmsm33["MAT_TUBE"];
				//bcls_rec->Tables["TMMSM33"].Rows[0]["MAT_TUBE"] = 1;
			}
			else
			{
				hmmsm33["HEAT_NO"] = tmmsm33["HEAT_NO"];
				/*if (hmmsm33.QueryCount("HEAT_NO") > 0)
				{
				strcpy(s.sysmsg, "该熔炼号方坯已经存在不能再次新增，请选择修改操作");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//n_count = 1;
			}


			//wzn_20170921 更新调用结构，先根据录入支数准备Info,统一调用各数据准备函数并调用主逻辑函数
			for (int i = 0; i < n_count; i++)
			{
				if (i > 0)
				{
					bcls_rec->Tables["TMMSM33"].Rows.Add();
					bcls_rec->Tables["TMMSM33"].Rows[i].Merge(bcls_rec->Tables["TMMSM33"].Rows[0]);
					bcls_rec->Tables["TMMSM33"].Rows[i]["SLAB_CUT_TIME"] = bcls_rec->Tables["TMMSM33"].Rows[0]["SLAB_CUT_TIME"].ToString();
					
					//第二次切割，清空命令信息
					for (int j = 1; j <= 12; j++)
					{
						bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)] = " ";
					}
					bcls_rec->Tables["TMMSM33"].Rows[i]["LSLAB_NO"] = " ";
					bcls_rec->Tables["TMMSM33"].Rows[i]["MAT_NO"] = " ";
				}
				//bcls_rec->Tables["TMMSM33_BK"].Rows.Add(); wzn_20170921剥夺其存在意义 
				//bcls_rec->Tables["TMMSM33_BK"].Rows[i].Merge(bcls_rec->Tables["TMMSM33"].Rows[0]);

			}
		}

		//bcls_rec->Tables["TMMSM33"].Rows.Clear();
		//for (int i = 0; i < bcls_rec->Tables["TMMSM33_BK"].Rows.get_Count(); i++){
		//	bcls_rec->Tables["TMMSM33"].Rows.Add();
		//	bcls_rec->Tables["TMMSM33"].Rows[i].Merge(bcls_rec->Tables["TMMSM33_BK"].Rows[i]);
		//}
		//Log::Trace("", "", "PRACT_COLL_MODE={0}", tmmsm33["PRACT_COLL_MODE"].ToString());
		//Log::Trace("", "", "MAT_NO={0}", tmmsm33["MAT_NO"].ToString());
		if (tmmsm33["MAT_NO"].ToString().Trim() == "")
		{
			/*ret = f_mmsm_get_matno(bcls_rec, bcls_ret, conn);*///产品化最初版
			/*ret = f_mmsm_matno_catch_sm(bcls_rec, bcls_ret, conn);*///产品化迭代版
			ret = f_mmsm_mat_no_ins(bcls_rec, bcls_ret, conn);//项目定制版
			if (ret < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		ret = f_mmsm_get_matwt(bcls_rec, bcls_ret, conn);
		if (ret < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}


		ret = f_mmsm33_pickslab(bcls_rec, bcls_ret, conn);
		if (ret < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		ret = f_mmsm33_check(bcls_rec, bcls_ret, conn);
		if (ret < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsm33.MergeFrom(bcls_rec->Tables["TMMSM33"].Rows[0]);

		for (int n = 0; n < bcls_rec->Tables["TMMSM33"].Rows.get_Count(); n++)
		{
			tmmsm33.Reset();

			tmmsm33.MergeFrom(bcls_rec->Tables["TMMSM33"].Rows[n]);
			tmmsm33["START_TIME"] = tmmsm33["SLAB_CUT_TIME"];
			tmmsm33["END_TIME"] = tmmsm33["SLAB_CUT_TIME"];
			

			tmmsm33.TrimOrBlank();
			tmmsm33.Print();
			tmmsm33["PROD_MAKER"] = tmmsm33["REC_CREATOR"];
			tmmsm33.Insert();
		}

		//ret = f_mmsm3301_proc(bcls_rec, bcls_ret, conn);
		ret = f_mmsm3301n_proc(bcls_rec, bcls_ret, conn);
		if (ret < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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
	if (doFlag < 0)
	{
		//Log::Trace("", __FUNCTION__, "******************输出传入数据开始**********************");
		//tmmsm33.Print();
		//Log::Trace("", __FUNCTION__, "******************输出传入数据结束**********************");
	}
	return doFlag;

}
