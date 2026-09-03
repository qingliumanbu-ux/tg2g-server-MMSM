/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-07-04
Description: 连铸产出材料信息新增
***********************************************************************/

#include "stdafx.h"



   //连铸铸坯产出实绩




#if defined _SYS_MMS || defined _SYS_MES  

#endif


int f_mm000501_proc(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);    //物料跟踪树
//int f_pmoucr_mm_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);   //工序产出组卷信息更新
int f_mmsm_sample_lot_no(EIClass *bcls_rec, CModel & tmmsm01_in, EIClass * bcls_ret, CDbConnection * conn); //获取试批号

int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

int f_mmsm_slab_dest(const CString& slab_plan_dest, CString& slab_dest_code, CDbConnection * conn);

int f_mmsm_get_theorywt(CDecimal MAT_ACT_THICK, CDecimal MAT_ACT_WIDTH, CDecimal MAT_ACT_LEN, CDecimal MAT_NUM, CDecimal &MAT_THEORY_WT);

int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号

int f_mm0016(CString StockNo, CString FactoryDiv, CString &SysCode, CDbConnection * conn);//根据厂别返回系统别

int f_mmsm_get_density(CString ST_NO, CDecimal& MAT_DENSITY,  CDbConnection* conn);//通过钢种计算密度



#if defined _LINE_HP
int f_mmhp0001_proc(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);//调用厚板程序生成厚板目的和工序档

int f_pshp_top_trace(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
#endif



/* =========================================================================
/// <remark>
/// <summary>
/// 接收铸坯产出实绩，新建材料主档（不包括写合同信息、写履历以及状态确定）
/// <para>根据产出铸坯，建立材料主档信息。</para>
/// <para>当同一实物材料分多次报送，不走本函数，走修改分支。  </para>
/// </summary>
/// <param name= "xxxx">输入参数</param>
/// <returns>成功:0</returns>
/// <returns>失败:-1</returns>
/// </remark>
========================================================================= */
BM2_FUNCTION_EXPORT
int f_mmsm01_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int ret = 0;
	int i;
	int blkNum = 0;
	int fetchRowCount;

	CString cs_seq_no = "";

	CString datetime = "";

	CString c_factory_div = ""; //厂别区分
	CDecimal  v_slab_len = 0;     //计划板坯长度
	CDecimal  v_slab_thick = 0;
	CDecimal  v_slab_width = 0;
	CDecimal  v_slab_max_len = 0; //计划板坯长度最大值
	CDecimal  v_slab_min_len = 0; //计划板坯长度最小值
	CDecimal  MAT_THEORY_WT = 0; //重量
	CString	cs_mm00_mat_track_no("");			//材料跟踪号的后4位流水号
	CString   matIdSeq = "";
	int i_cut_seq = 0;
	int i_count = 0;
	CString ch_cc_mach_no = "";
	CString code_desc_3_content = "";
	CString sqlstr = "";
	CString sqlstr_ps10 = "";
	CString v_order_type = "";
	CString v_slab_dest_code = "";
	CString v_judge_code = "";
	CString v_backlog_type = "";
	CString v_fin_st_no = "";
	CString v_sys_code = "";
	CString v_mat_shape_flag = "";

	CString v_batch = "";//批次号
	CString v_ponoslab_col = "";//需要更新的ponoslab

	CModel tmmsm01("TMMSM01");
	CModel tmmsm33("TMMSM33");
	CModel tmmsm96("TMMSM96");
	CModel tpssm03("TPSSM03");
	CModel tpssm01("TPSSM01");
	CModel tqmts0x("TQMTS0X");
	CModel tqmts08("TQMTS08");
	CModel twmsmpz("TWMSMPZ");

	

    #if defined _SYS_MMS || defined _SYS_MES 
	CModel tmm0005("TMM0005");
    #endif
	
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_inq_ps10(conn);
	CDbCommand cmd_inq_tpssm03(conn);
	CDbCommand cmd_inq_tpssm11(conn);
	CDbCommand cmd_inq_tqmts23(conn);
	CDbCommand cmd_inq_tqmts0x(conn);
	

	
	EIClass bcls_rec_QM17;//材料质量封锁
	bcls_rec_QM17.Tables[0].set_TableName("MM0099");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	//bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "HOLD_REMARK");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_REMARK");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_MAKER");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_TIME");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "HOLD_CAUSE_CODE");
	bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");

	EIClass bcls_rec_PSHP;//材料质量封锁
	bcls_rec_PSHP.Tables[0].set_TableName("PSHP");
	bcls_rec_PSHP.Tables[0].Columns.Add(DT_STRING, "OP_TYPE");
	bcls_rec_PSHP.Tables[0].Columns.Add(DT_STRING, "MAT_NO");

	try
	{
		/*判断传入参数块是否存在*/
		blkNum = bcls_rec->Tables.IndexOf("TMMSM33_IC");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 TMMSM33_IC 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/*物料跟踪履历用，f_mmsm99函数用*/
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}
		bcls_rec->Tables["MM0099"].Rows.Clear();
		/*f_mmhp0001_proc()函数用*/
		blkNum = bcls_rec->Tables.IndexOf("MMHP0001");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMHP0001");
		}
		bcls_rec->Tables["MMHP0001"].Rows.Clear();

		//增加MM000501块, f_mm000501_proc()函数用
		blkNum = bcls_rec->Tables.IndexOf("MM000501");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM000501");
		}
		bcls_rec->Tables["MM000501"].Rows.Clear();
		//add by xp  2014/08/26  
		/*工序产出组卷信息更新*/
		blkNum = bcls_rec->Tables.IndexOf("PMOU");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("PMOU");
		}
		bcls_rec->Tables["PMOU"].Rows.Clear();

		//获取去向
		tmmsm33["SLAB_PLAN_DEST"] = bcls_rec->Tables["TMMSM33_IC"].Rows[0]["SLAB_PLAN_DEST"].ToString();
		tmmsm33["FACTORY_DIV"] = bcls_rec->Tables["TMMSM33_IC"].Rows[0]["FACTORY_DIV"].ToString();
		tmmsm33["SLAB_TYPE"] = bcls_rec->Tables["TMMSM33_IC"].Rows[0]["SLAB_TYPE"].ToString();
		tmmsm33["INGOT_CODE"] = bcls_rec->Tables["TMMSM33_IC"].Rows[0]["INGOT_CODE"].ToString();

		////Log::Info("", __FUNCTION__, "传入参数,tmmsm33["SLAB_PLAN_DEST"] =[{0}]", tmmsm33["SLAB_PLAN_DEST"].ToString());
		////Log::Info("", __FUNCTION__, "传入参数,tmmsm33["FACTORY_DIV"] =[{0}]", tmmsm33["FACTORY_DIV"].ToString());
		////Log::Info("", __FUNCTION__, "传入参数,tmmsm33["SLAB_TYPE"] =[{0}]", tmmsm33["SLAB_TYPE"].ToString());
		////Log::Info("", __FUNCTION__, "传入参数,tmmsm33.SLAB_TYPExxxxxxxxxxxxxxxxxxx=[{0}]", tmmsm33["SLAB_TYPE"].ToString());


		doFlag = f_mmsm_slab_dest(tmmsm33["SLAB_PLAN_DEST"].ToString(), v_slab_dest_code, conn);
		if (doFlag < 0 || v_slab_dest_code.Trim() == "")
		{
			sprintf(s.msg, "获取去向出错!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//Log::Trace("", "", "v_slab_dest_code={0}", v_slab_dest_code);


		//获取系统别
		doFlag = f_mm0016("", tmmsm33["FACTORY_DIV"].ToString(), v_sys_code, conn);
		if (doFlag < 0)
		{
			sprintf(s.msg, "根据厂别获取系统别出错!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		//Log::Trace("", "", "v_sys_code={0}", v_sys_code);

		//太钢定制    mfj  20240113  
		//通过代码转化实现材料形态赋值
		/*sqlstr = "SELECT CODE_DESC_4_CONTENT "
			"  FROM TEP0002 "
			"  WHERE CODE_CLASS 	=  'PSA6'"
			"  AND CODE =  @tmmsm33.SLAB_TYPE ";


		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Clear();
		cmd_sql.Parameters.Set("tmmsm33.SLAB_TYPE", tmmsm33["SLAB_TYPE"].ToString());
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			v_mat_shape_flag = cmd_sql.GetString(1);

		}

		
		cmd_sql.Close();*/
		
		v_mat_shape_flag = "1";//板坯

		//Log::Trace("", "", "v_mat_shape_flag={0}", v_mat_shape_flag);

		//Log::Trace("", "", "tmmsm33["INGOT_CODE"] ={0}", tmmsm33["INGOT_CODE"].ToString());

		//Log::Trace("", "", "tmmsm33["SLAB_TYPE"] ={0}", tmmsm33["SLAB_TYPE"].ToString());

		//模铸根据定型取质量表中的规格
		if (tmmsm33["SLAB_TYPE"].ToString().Trim() == "5")
		{

			sqlstr = "SELECT SLAB_THICK, SLAB_WIDTH, SLAB_LEN "
				"  FROM TQMTMD9 "
				"  WHERE BILLET_TYPE = '5'"
				"  AND INGOT_CODE =  @tmmsm33.INGOT_CODE ";


			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Clear();
			cmd_sql.Parameters.Set("tmmsm33.INGOT_CODE", tmmsm33["INGOT_CODE"].ToString());
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				v_slab_thick = cmd_sql.GetDecimal(1);
				v_slab_width = cmd_sql.GetDecimal(2);
				v_slab_len = cmd_sql.GetDecimal(3);
			}
			cmd_sql.Close();

			//Log::Trace("", "", "tmmsm33["SLAB_THICK"] ={0}", tmmsm33["SLAB_THICK"].ToDecimal());
			//Log::Trace("", "", "tmmsm33["SLAB_WIDTH"] ={0}", tmmsm33["SLAB_WIDTH"].ToDecimal());
			//Log::Trace("", "", "tmmsm33["SLAB_LEN"] ={0}", tmmsm33["SLAB_LEN"].ToDecimal());

		}


		bcls_rec->Tables["MM0099"].Rows.Clear();
		for (int i = 0; i < bcls_rec->Tables["TMMSM33_IC"].Rows.get_Count(); i++){
			//----------------------------------------------
			//获取传入参数（块名MMSM33，产出铸坯信息TMMSM33），单记录处理
			tmmsm33.Reset();
			tmmsm33.MergeFrom(bcls_rec->Tables["TMMSM33_IC"].Rows[i]);

			v_batch = "";//将批次号置空
			v_ponoslab_col = "";//将命令坯号集合置空

			Log::Trace("", "", "tmmsm33.SLAB_CUT_TIME333 ={0}", tmmsm33["SLAB_CUT_TIME"].ToString());

			if (tmmsm33["SLAB_TYPE"].ToString().Trim() == "5")
			{
				tmmsm33["SLAB_LEN"] = v_slab_len;
				tmmsm33["SLAB_THICK"] = v_slab_thick;
				tmmsm33["SLAB_WIDTH"] = v_slab_width;
			}
			//tmmsm33.Print();

			datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


			//----------------------------------------------
			//根据命令板坯获取相关信息。当产出铸坯为长坯(厚板的)，以第一块命令铸坯信息为准。
			//modify by xp 2017/05/11 对应的命令板坯号只要有一个是有合同的，长坯就按照合同材处理
			//如果没有值，则为余材
			tpssm03.Reset();
			tpssm01.Reset();
			tmmsm01.Reset();
			////Log::Info("", __FUNCTION__, "传入参数,tmmsm33["PONO_SLAB_1"] =[{0}]", tmmsm33["PONO_SLAB_1"].ToString());//命令板坯号
			////Log::Info("", __FUNCTION__, "传入参数,tmmsm33["PONO_SLAB_2"] =[{0}]", tmmsm33["PONO_SLAB_2"].ToString());//命令板坯号


			//查询tpssm01表，获取标记，区分后备制造命令的炉次  连铸产出时按余才产出，不需要匹配命令板坯_防止错误
			tpssm01["PONO"] = tmmsm33["PONO"];
			tpssm01.Query("PONO");
			//当为1时，为后备制造命令，需将命令坯删掉，并更新TMMSM01表APP_TYPE字段
			if (tpssm01["APP_TYPE"].ToString().Trim() == "1")
			{
				for (int i = 1; i <= tmmsm33["FIX_SLAB_NUM"].ToDecimal(); i++)
				{
					v_ponoslab_col += ",PONO_SLAB_" + CConvert::ToString(i);
					tmmsm33["PONO_SLAB_" + CConvert::ToString(i)] = "";
				}
				tmmsm33["FIX_SLAB_NUM"] = 0;
				
				v_ponoslab_col += ",FIX_SLAB_NUM";
				v_ponoslab_col = v_ponoslab_col.SubstringNE(1);

				Log::Trace("", "", "v_ponoslab_col={0}", v_ponoslab_col);
				tmmsm33.TrimOrBlank();
				tmmsm33.Update(v_ponoslab_col, "MAT_NO");

				tmmsm01["APP_TYPE"] = "1";
			}


			if (tmmsm33["PONO_SLAB_1"].ToString().Trim() == "") //余材
			{
				////Log::Info("", __FUNCTION__, "传入参数,余材的PONO=[{0}]", tmmsm33["PONO"].ToString());

				sqlstr = "SELECT * "
					"  FROM TPSSM03 "
					" WHERE PONO = @pono";

				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("pono", tmmsm33["PONO"].ToString().Trim());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(tpssm03); //把数据都压在头文件里面
				}
				else
				{
					//对应的命令铸坯不存在
					////Log::Info("", __FUNCTION__, "产出铸坯[tmmsm33["MAT_NO"] ={0}]对应的命令铸坯[tmmsm33["PONO_SLAB_1"] ={1}]信息不存在", tmmsm33["MAT_NO"].ToString(), tmmsm33["PONO_SLAB_1"].ToString());
					/*strcpy(s.msg, "产出铸坯对应的命令铸坯信息不存在。");
					cmd_inq.Close();
					throw CApplicationException(-1, s.msg, log.Location);*/
					sqlstr_ps10 = " select * from vpssm10 where pono ='" + tmmsm33["PONO"].ToString().Trim() + "' ";
					Log::Trace("", "", "sqlstr_ps10={0}", sqlstr_ps10);
					cmd_inq_ps10.SetCommandText(sqlstr_ps10);
					cmd_inq_ps10.ExecuteReader();
					if (cmd_inq_ps10.Read())
					{
						cmd_inq_ps10.Fetch(tpssm03);
					}
					cmd_inq_ps10.Close();
				}
				cmd_inq.Close();
				tpssm03["ORDER_NO"] = " ";
				//tpssm03["HOT_CHARGE_FLAG"] = "0";
				tpssm03["HOT_SEND_FLAG"] = "0";
				tpssm03["SLAB_THICK"] = 0;
				tpssm03["SLAB_WIDTH"] = 0;
				tpssm03["SLAB_LEN"] = 0;
				tpssm03["SLAB_WT"] = 0;
				tpssm03["SLAB_DEST"] = " ";
			}
			else //命令材
			{
				////Log::Info("", __FUNCTION__, "传入参数,命令材的PONO=[{0}]", tmmsm33["PONO"].ToString());
				////Log::Info("", __FUNCTION__, "传入参数,命令材的lslab_no=[{0}]", tmmsm33["LSLAB_NO"].ToString());

				//add by xp   找到第一块有合同的命令板坯，而不是取第一块
				if (tmmsm33["FIX_SLAB_NUM"].ToDecimal() > 1)
				{
					sqlstr = "SELECT SLAB_NO "
						"  FROM TPSSM03 "
						"  WHERE PONO 	=   @pono"
						"  AND  LSLAB_NO = @lslab_no "
						"  AND  ORDER_NO <>' '   ORDER BY  LSLAB_NO,slab_no";
				}
				else
				{
					sqlstr = "SELECT SLAB_NO "
						"  FROM TPSSM03 "
						"  WHERE PONO 	=   @pono"
						"  AND  SLAB_NO = @slab_no  ORDER BY  LSLAB_NO,slab_no"
						//"  AND  ORDER_NO <>' '"
						;
				}


				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("pono", tmmsm33["PONO"].ToString().Trim());
				cmd_inq.Parameters.Set("lslab_no", tmmsm33["LSLAB_NO"].ToString().Trim());
				cmd_inq.Parameters.Set("slab_no", tmmsm33["PONO_SLAB_1"].ToString().Trim());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					//cmd_inq.Fetch(tpssm03); //把数据都压在头文件里面
					tpssm03["SLAB_NO"] = cmd_inq.GetString(1);
				}
				else
				{
					tpssm03["SLAB_NO"] = tmmsm33["PONO_SLAB_1"];
				}
				cmd_inq.Close();



				//读取命令铸坯信息
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:        // Oracle 数据库
				default:
					sqlstr = CString(
						" SELECT * FROM TPSSM03 "
						"  WHERE SLAB_NO           = @slab_no "

						);
					break;
				}

				cmd_inq_tpssm03.SetCommandText(sqlstr);

				if (tpssm03["SLAB_NO"].ToString().Trim() != "")
				{
					cmd_inq_tpssm03.Parameters.Set("slab_no", tpssm03["SLAB_NO"].ToString().Trim());
				}
				else
				{
					cmd_inq_tpssm03.Parameters.Set("slab_no", tmmsm33["PONO_SLAB_1"].ToString().Trim());
				}

				cmd_inq_tpssm03.ExecuteReader();
				if (cmd_inq_tpssm03.Read())
				{

					cmd_inq_tpssm03.Fetch(tpssm03); //把数据都压在头文件里面

				}
				else
				{
					//对应的命令铸坯不存在
					////Log::Info("", __FUNCTION__, "产出铸坯[tmmsm33["MAT_NO"] ={0}]对应的命令铸坯[tmmsm33["PONO_SLAB_1"] ={1}]信息不存在", tmmsm33["MAT_NO"].ToString(), tmmsm33["PONO_SLAB_1"].ToString());
				/*	strcpy(s.msg, "产出铸坯对应的命令铸坯信息不存在。");
					cmd_inq_tpssm03.Close();
					throw CApplicationException(-1, s.msg, log.Location);*/
					sqlstr_ps10 = " select * from vpssm10 where pono ='" + tmmsm33["PONO"].ToString().Trim() + "' ";
					Log::Trace("", "", "sqlstr_ps10={0}", sqlstr_ps10);
					cmd_inq_ps10.SetCommandText(sqlstr_ps10);
					cmd_inq_ps10.ExecuteReader();
					if (cmd_inq_ps10.Read())
					{
						cmd_inq_ps10.Fetch(tpssm03);
					}
					cmd_inq_ps10.Close();
				}
				cmd_inq_tpssm03.Close();
			}

			////Log::Info("", __FUNCTION__, "传入参数,tpssm03["SLAB_NO"] =[{0}]", tpssm03["SLAB_NO"].ToString());
			////Log::Info("", __FUNCTION__, "传入参数,tmmsm33["PONO_SLAB_1"] =[{0}]", tmmsm33["PONO_SLAB_1"].ToString());
			////Log::Info("", __FUNCTION__, "传入参数,tpssm03["ORDER_NO"] =[{0}]", tpssm03["ORDER_NO"].ToString());//合同号


			
#if defined(_SYS_PES)
			////获取精炼路径
			sqlstr = "SELECT REFINE_ROUTE_CODE "
				"  FROM TPSSM11 "
				" WHERE PONO = @pono";

			////Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

			cmd_inq_tpssm11.SetCommandText(sqlstr);
			cmd_inq_tpssm11.Parameters.Set("pono", tmmsm33["PONO"].ToString().Trim());
			cmd_inq_tpssm11.ExecuteReader();
			if (cmd_inq_tpssm11.Read())
			{
				tmmsm01["REFINE_ROUTE_CODE"] = cmd_inq_tpssm11.GetString(1);
			}

			cmd_inq_tpssm11.Close();
#endif  //_SYS_PES
			//***************切断实绩接收创建板坯主档**********************

			tmmsm01["REC_CREATOR"] = tmmsm33["REC_CREATOR"];
			tmmsm01["REC_CREATE_TIME"] = tmmsm33["REC_CREATE_TIME"];
			tmmsm01["FACTORY_DIV"] = tmmsm33["FACTORY_DIV"];
			tmmsm01["FACTORY_PROD"] = tmmsm33["FACTORY_DIV"];
			tmmsm01["HEAT_NO"] = tmmsm33["HEAT_NO"];
			tmmsm01["PONO"] = tmmsm33["PONO"];
			tmmsm01["PROD_SHIFT_NO"] = tmmsm33["PROD_SHIFT_NO"];
			tmmsm01["PROD_SHIFT_GROUP"] = tmmsm33["PROD_SHIFT_GROUP"];
			tmmsm01["MAT_NO"] = tmmsm33["MAT_NO"];
			tmmsm01["PROD_MAKER"] = tmmsm33["PROD_MAKER"];
			tmmsm01["CAST_NO"] = tmmsm33["CAST_NO"];
			tmmsm01["CAST_DIV_NO"] = tmmsm33["CAST_DIV_NO"];
			tmmsm01["ADJUST_WIDTH_MARK"] = tmmsm33["ADJUST_WIDTH_MARK"];
			tmmsm01["SLAB_HEAD_WIDTH"] = tmmsm33["SLAB_HEAD_WIDTH"];
			tmmsm01["SLAB_TAIL_WIDTH"] = tmmsm33["SLAB_TAIL_WIDTH"];
			tmmsm01["SLAB_TAPPER_WIDTH_START"] = tmmsm33["SLAB_TAPPER_WIDTH_START"];
			tmmsm01["SLAB_TAPPER_WIDTH_LEN"] = tmmsm33["SLAB_TAPPER_WIDTH_LEN"];
			tmmsm01["INGOT_CODE"] = tmmsm33["INGOT_CODE"];
			tmmsm01["IF_MIX_POUR"] = tmmsm33["IF_MIX_POUR"];
			tmmsm01["LOGISTICS_STATUS"] = "0";
			tmmsm01["MANAGE_FLAG"] = tmmsm33["MANAGE_FLAG"];
			tmmsm01["NO_SLAB_CAUSE"] = tmmsm33["NO_SLAB_CAUSE"];//未挂命令坯原因   mfj  太钢定制  20240103


			//获取物料跟踪号
			doFlag = f_mm0011("MM00_MAT_TRACK_NO", 4, cs_seq_no, conn);
			if (doFlag < 0 || cs_seq_no.Trim() == "")
			{
				sprintf(s.msg, "获取 生产流水号 失败，请查看数据库sequence【MM00_MAT_TRACK_NO】是否正常!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			tmmsm01["MAT_TRACK_NO"] = datetime + cs_seq_no.Trim();	//物料跟踪号 流水号

			tmmsm01["PASS_BACKLOG_SEQ_NO"] = 1;

			tmmsm01["MAT_LINE_TYPE"] = "SM";                         /*物料产线类型*/
			//tmmsm01["RAW_ORIGIN"] = "2";                          /*原料来源代码M00D：2-机组产出 */
			//tmmsm01["RAW_ORIGIN"] = tmmsm01["HEAT_NO"].ToString().SubstringNE(2, 2);			/*供后道产线使用*/
			tmmsm01["RAW_ORIGIN"] = "100000";//暂定内供，后续需要根据合同大类来区分   mfj  20231124
			tmmsm01["MAT_ORIGIN"] = "2";                          /*材料来源代码(1-外购；2-机组产出；3-回退；5-清盘库；)*/
			tmmsm01["MAT_ORIGIN_DETAIL"] = "0" + tmmsm33["STATION_NO"].ToString();     /*材料来源细分：连铸机号 */
			tmmsm01["SLAB_COLD_HOT_FLAG"] = "H";                          /*板坯冷热标志*/
			
			tmmsm01["MAT_THICK"] = tmmsm33["SLAB_THICK"];            /*板坯宽度   根据用户要求，来调整用实际值还是用命令值*/
			tmmsm01["MAT_WIDTH"] = tmmsm33["SLAB_WIDTH"];            /*板坯厚度*/
			tmmsm01["MAT_LEN"] = tmmsm33["SLAB_LEN"];              /*板坯长度 */
			tmmsm01["MAT_ACT_THICK"] = tmmsm33["SLAB_THICK"];            /*板坯实际宽度*/
			tmmsm01["MAT_ACT_WIDTH"] = tmmsm33["SLAB_WIDTH"];            /*板坯实际厚度*/
			tmmsm01["MAT_ACT_LEN"] = tmmsm33["SLAB_LEN"];              /*板实际坯长度*/

			

			tmmsm01["UNIT_CODE"] = tmmsm33["STATION_ID"].ToString() + tmmsm33["STATION_NO"].ToString(); /*机组代码 产出铸机号*/
			tmmsm01["DEV_CODE"] = tmmsm33["STATION_ID"].ToString() + tmmsm33["STATION_NO"].ToString(); /*机组代码 产出铸机号*/
			tmmsm01["ST_NO"] = tmmsm33["ST_NO"];                /*出钢记号*/
			tmmsm01["HOT_CHARGE_FLAG"] = tpssm03["HOT_CHARGE_FLAG"];
			tmmsm01["HOT_SEND_FLAG"] = tpssm03["HOT_SEND_FLAG"];
			tmmsm01["MAT_SHAPE_FLAG"] = v_mat_shape_flag;

			tmmsm01["MAT_KIND"] = "SM";         //物料种类：炼钢浇铸出的都为SM
			tmmsm01["MAT_DESTION"] = tpssm03["SLAB_DEST"];        /*板坯去向*/
			tmmsm01["PREC_SLAB_NO"] = tmmsm33["PONO_SLAB_1"];           /*预定板坯号*/
			tmmsm01["PONO_SLAB"] = tmmsm33["PONO_SLAB_1"];           /*命令板坯号*/
			tmmsm01["PONO_SLAB_1"] = tmmsm33["PONO_SLAB_1"];           /*命令板坯号*/
			tmmsm01["PONO_SLAB_2"] = tmmsm33["PONO_SLAB_2"];           /*命令板坯号*/
			tmmsm01["PONO_SLAB_3"] = tmmsm33["PONO_SLAB_3"];           /*命令板坯号*/
			tmmsm01["PONO_SLAB_4"] = tmmsm33["PONO_SLAB_4"];           /*命令板坯号*/
			tmmsm01["PONO_SLAB_5"] = tmmsm33["PONO_SLAB_5"];           /*命令板坯号*/
			tmmsm01["PONO_SLAB_6"] = tmmsm33["PONO_SLAB_6"];           /*命令板坯号*/
			tmmsm01["PONO_SLAB_7"] = tmmsm33["PONO_SLAB_7"];           /*命令板坯号*/
			tmmsm01["PONO_SLAB_8"] = tmmsm33["PONO_SLAB_8"];           /*命令板坯号*/
			tmmsm01["PONO_SLAB_9"] = tmmsm33["PONO_SLAB_9"];           /*命令板坯号*/
			tmmsm01["PONO_SLAB_10"] = tmmsm33["PONO_SLAB_10"];           /*命令板坯号*/
			tmmsm01["PONO_SLAB_11"] = tmmsm33["PONO_SLAB_11"];           /*命令板坯号*/
			tmmsm01["PONO_SLAB_12"] = tmmsm33["PONO_SLAB_12"];           /*命令板坯号*/
			tmmsm01["FIX_SLAB_NUM"] = tmmsm33["FIX_SLAB_NUM"];
			tmmsm01["LSLAB_NO"] = tmmsm33["LSLAB_NO"];

			//长短坯标记    后面需在画面加按钮让用户修改   mfj   20231124 
			if (tmmsm01["FIX_SLAB_NUM"].ToDecimal() >= 2)
			{
				tmmsm01["SLAB_FLAG"] = "L";                          /*长板坯标记C1*/
			}
			else
			{
				tmmsm01["SLAB_FLAG"] = "S";
			}
			tmmsm01["SM_PLAN_NO"] = tmmsm33["SM_PLAN_NO"];//计划号  
			tmmsm01["SM_PLAN_NOL2"] = tmmsm33["SM_PLAN_NOL2"];//计划号  
			tmmsm01["MAT_ACT_DESTION"] = tmmsm01["MAT_DESTION"];//实际去向  取计划去向
			
			tmmsm01["PROC_COUNT"] = 1;
			//材料目标值，在最终按命令要求产出时，才有意义。长尺坯、余材下无意义
			tmmsm01["ORDER_NO"] = tpssm03["ORDER_NO"];               /*合同号           */
			tmmsm01["MAT_TARG_THICK"] = tpssm03["SLAB_THICK"];                    /*材料目标厚度*/
			tmmsm01["MAT_TARG_WIDTH"] = tpssm03["SLAB_WIDTH"];                    /*材料目标宽度*/
			tmmsm01["MAT_TARG_LEN"] = tmmsm33["SLAB_LEN"];

			//太钢定制，二级有传目标长度，若有值，则覆盖01表的目标长度
			if (tmmsm33["MAT_TARG_LEN"].ToDecimal() > 0)
			{
				tmmsm01["MAT_TARG_LEN"] = tmmsm33["MAT_TARG_LEN"];
			}
			tmmsm01["L2_THEORY_WT"] = tmmsm33["L2_THEORY_WT"].ToDecimal().Round(3);//二级理论量
			tmmsm01["MAT_ACT_WT"] = 0;//实际重量为太钢的系统重量  系统重量只在收货后有值 mfj  20240305
			tmmsm01["L3_CALTHEROY_WT"] = tmmsm33["SLAB_WT"].ToDecimal().Round(3);//三级理论量
			tmmsm01["REAL_TIME_WT"] = tmmsm33["SLAB_WT"].ToDecimal().Round(3);//实时重量
			tmmsm01["PREC_ST_NO"] = tmmsm33["ST_NO"];           /*预定出钢记号：参数传入时必须赋值*/
			tmmsm01["PROD_TIME"] = tmmsm33["SLAB_CUT_TIME"];   /*生产时刻 */
			tmmsm01["SLAB_CUT_TIME"] = tmmsm33["SLAB_CUT_TIME"];   /*生产时刻 */

			tmmsm01["MAT_NUM"] = tmmsm33["MAT_TUBE"];
			tmmsm01["MAT_TUBE"] = tmmsm33["MAT_TUBE"];
			tmmsm01["MAT_NUM_CUT"] = tmmsm33["MAT_TUBE"];

			tmmsm01["SLAB_PLACE_CODE"] = tmmsm33["SLAB_PLACE_CODE"];    /*板坯位置代码*/
			tmmsm01["REPAIR_FLAG"] = "0";                  /*返修标记C1*/
			tmmsm01["HOLD_FLAG"] = "0";                  /*封锁标记C1*/
			tmmsm01["SURFACE_DECIDE_CODE"] = "1";              /*表面判定代码C1*/

			tmmsm01["SURFACE_DECIDE_TIME"] = tmmsm01["REC_CREATE_TIME"];         /*表面判定时间*/
			tmmsm01["SURFACE_DECIDE_MAKER"] = " ";             /*表面判定责任者*/
			tmmsm01["PCH_JUDGE_CODE"] = "0";              /*性能判定代码C1*/
			tmmsm01["COMPLEX_DECIDE_CODE"] = "0";              /*综合判定代码C1*/
			tmmsm01["SLABTOP_FLAG"] = "0";              /*板坯TOP点确认标志*/

			tmmsm01["IN_FLAG"] = "0";              /*入库标记*/
			tmmsm01["TRANSFER_FLAG"] = "0";              /*转库计划标记*/
			tmmsm01["PRODUCT_PACK_FLAG"] = "0";              /*成品包装标志*/
			tmmsm01["CONFM_FLAG"] = "0";              /*准发确认标记*/
			tmmsm01["APP_DECIDE_FLAG"] = "0";              /*现货申报标记*/
			tmmsm01["MEASURE_WT_FLAG"] = tmmsm33["MEASURE_WT_FLAG"];
			//tmmsm01["IN_MAT_NO"] = tmmsm01["MAT_NO"];
			Log::Info("", __FUNCTION__, "tmmsm01.MEASURE_WT_FLAG =[{0}]", tmmsm01["MEASURE_WT_FLAG"].ToString());

			//tmmsm01["MAT_NO_OLD"] = tmmsm01["MAT_NO"];
			//tmmsm01["OLD_HEAT_NO"] = tmmsm01["HEAT_NO"];
			//tmmsm01["OLD_PONO"] = tmmsm01["PONO"];
			tmmsm01["IF_TRANSFER"] = tmmsm33["IF_TRANSFER"];
			tmmsm01["STOCK_PLACE_NO"] = "GD";
			tmmsm01["STRAND_NO"] = tmmsm33["STRAND_NO"];
			tmmsm01["CC_NO"] = tmmsm33["PROC_NO"];

			Log::Info("", __FUNCTION__, "tmmsm01.CC_NO =[{0}]", tmmsm01["CC_NO"].ToString());
			//20151020 增加物料工序信息获取
			tmmsm01["PSC"] = tpssm03["PSC"];
			tmmsm01["PROD_CODE"] = tmmsm01["PSC"].ToString().Trim().SubstringNE(0, 2);
			tmmsm01["PROD_CLASS_CODE"] = tmmsm01["PSC"].ToString().Trim().SubstringNE(0, 1);

			tmmsm01["MSC"] = tpssm03["MSC"];
			tmmsm01["APN"] = tpssm03["APN"];
			tmmsm01["CASTING_PURPOSE"] = tmmsm01["APN"];//铸坯用途
			tmmsm01["SG_SIGN"] = tpssm03["SG_SIGN"];
			tmmsm01["SG_STD"] = tpssm03["SG_STD"];
			tmmsm01["WHOLE_BACKLOG"] = tpssm03["WHOLE_BACKLOG"];
			tmmsm01["WHOLE_BACKLOG_NO"] = tpssm03["WHOLE_BACKLOG_NO"];
			tmmsm01["WHOLE_BACKLOG_CODE"] = tpssm03["WHOLE_BACKLOG_CODE"];
			tmmsm01["WHOLE_BACKLOG_SEQ"] = tpssm03["WHOLE_BACKLOG_SEQ"];
			//tmmsm01["BASE_CODE"] = tpssm03["BASE_CODE"];
			//Log::Info("", __FUNCTION__, "tmmsm01.BASE_CODE =[{0}]", tmmsm01["BASE_CODE"].ToString());
			//后全程工序顺序号N2
			if (tmmsm01["WHOLE_BACKLOG"].ToString().Trim() != "")
			{
				tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"] = tmmsm01["WHOLE_BACKLOG_SEQ"].ToDecimal() + 1;
				tmmsm01["NEXT_WHOLE_BACKLOG_CODE"] = tmmsm01["WHOLE_BACKLOG"].ToString().SubstringNE((tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"].ToDecimal().ToInt16() * 2) - 2, 2);
			}

			////Log::Info("", __FUNCTION__, "全程工序途径码whole_backlog=[{0}]", tmmsm01["WHOLE_BACKLOG"].ToString());
			////Log::Info("", __FUNCTION__, "制程号WHOLE_BACKLOG_NO=[{0}]", tmmsm01["WHOLE_BACKLOG_NO"].ToDecimal());
			////Log::Info("", __FUNCTION__, "全程工序代码WHOLE_BACKLOG_CODE=[{0}]", tmmsm01["WHOLE_BACKLOG_CODE"].ToString());
			////Log::Info("", __FUNCTION__, "全程工序顺序号WHOLE_BACKLOG_SEQ=[{0}]", tmmsm01["WHOLE_BACKLOG_SEQ"].ToDecimal());
			////Log::Info("", __FUNCTION__, "后全程工序代码next_whole_backlog_code=[{0}]", tmmsm01["NEXT_WHOLE_BACKLOG_CODE"].ToString());
			////Log::Info("", __FUNCTION__, "后全程工序顺序号NEXT_WHOLE_BACKLOG_SEQ=[{0}]", tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"].ToDecimal());

			//(材料)通过工序序列号N4	  	
			tmmsm01["PASS_BACKLOG_SEQ_NO"] = 1;

			//实际全程工序途径码C50 	
			tmmsm01["WHOLE_BACKLOG_ACT"] = tmmsm01["WHOLE_BACKLOG_CODE"];


			//将初始_字段赋实际值   mfj  20231127
			//初始值产出时先不赋值，等到盘库修改过渡坯归属时赋值，且只赋一次
			//tmmsm01["INITIAL_ST_NO"] = tmmsm01["ST_NO"];//初始出钢记号
			////tmmsm01["INITIAL_STD_SG_CODE"] = tmmsm01["SG_STD"];//初始标准牌号代码
			//tmmsm01["INITIAL_SG_SIGN"] = tmmsm01["SG_SIGN"];//初始牌号(钢级)
			//tmmsm01["INITIAL_SG_STD"] = tmmsm01["SG_STD"];//初始标准
			////tmmsm01["INITIAL_SIGN_CODE"] = tmmsm01[""];//初始钢级代码
			//tmmsm01["INITIAL_ORDER_NO"] = tmmsm01["ORDER_NO"];//初始合同号
			////tmmsm01["INITIAL_FIN_CUST_CODE"] = tmmsm01[""];//初始最终用户代码
			//tmmsm01["INITIAL_MSC"] = tmmsm01["MSC"];//初始冶金规范码
			////tmmsm01["INITIAL_MSC_LINE_NO"] = tmmsm01[""];//初始产线号
			//tmmsm01["INITIAL_PSC"] = tmmsm01["PSC"];//初始产品规范码
			//tmmsm01["INITIAL_APN"] = tmmsm01["APN"];//初始产品最终用途码
			////tmmsm01["INITIAL_WKM"] = tmmsm01[""];//初始客户特殊要求码
			////tmmsm01["INITIAL_APPLY_SG_SIGN"] = tmmsm01[""];//初始适用牌号
			////tmmsm01["INITIAL_PRICE"] = tmmsm01[""];//初始差价
			//tmmsm01["INITIAL_HEAT_NO"] = tmmsm01["HEAT_NO"];//初始熔炼号
			////tmmsm01["INITIAL_SG_SIGN_HR"] = tmmsm01[""];//初始牌号(钢级)_热轧形态

			tmmsm01["SURF_QUALITY"] = "47";//表面质量  
			tmmsm01["RCV_MAT_FLAG"] = "N";//收货标记赋默认值 N  未收货  W等待  S收货成功

			//将系数存入主档表
			tmmsm01["COE_A"] = tmmsm33["COE_A"];
			tmmsm01["COE_B"] = tmmsm33["COE_B"];
			tmmsm01["C_DIV"] = tmmsm33["C_DIV"];//获取碳锈区分
			//位置 给默认值SYA    mfj  20240320 
			tmmsm01["STOCK_NO"] = "SYA";

			//余材原因
			tmmsm01["REMAINDER_REASON"] = tmmsm33["NO_SLAB_CAUSE"];

			

			tmmsm01["PRINT_NO"] = tmmsm33["PRINT_NO"];
			Log::Info("", __FUNCTION__, "tmmsm01.PRINT_NO =[{0}] [{1}]", tmmsm01["PRINT_NO"].ToString(), s.userid);
			//喷印号  炉号+钢种代码+流号+切割顺序号+连浇序号
			CString user_id = s.userid;
			if (tmmsm01["PRINT_NO"].ToString().Trim() == "" && user_id != "xcom")
			{
				
				CString cut_no = "";//切割顺序号
				CString v_strand_no = tmmsm01["STRAND_NO"].ToString().Trim();//流号
				if (tmmsm01["SLAB_PLACE_CODE"].ToString().Trim() == "B")//头坯
				{
					//当为头坯单流时，顺序号为00 ,双流为AA
					if (v_strand_no == "Z" || v_strand_no == "A" || v_strand_no == "B" )
					{
						cut_no = "00";
					}
					else if ( v_strand_no == "C"|| v_strand_no == "D" || v_strand_no == "E" || v_strand_no == "F")
					{
						cut_no = "AA";
					}
				}
				else if (tmmsm01["SLAB_PLACE_CODE"].ToString().Trim() == "T")//尾坯
				{
					//当为尾坯单流时，顺序号为99 ,双流为ZZ
					if (v_strand_no == "Z" || v_strand_no == "A" || v_strand_no == "B" )
					{
						cut_no = "99";
					}
					else if ( v_strand_no == "C"|| v_strand_no == "D" || v_strand_no == "E" || v_strand_no == "F")
					{
						cut_no = "ZZ";
					}
				}
				
				
				int len_no =  tmmsm01["CAST_DIV_NO"].ToDecimal().Round(0).ToString().GetLength();
				CString cast_div_num = "";
				if (cut_no.Trim() != "")//当为头尾坯时
				{
					if (len_no == 1)
					{
						cast_div_num = "00";
					}
					else if (len_no == 2)
					{
						cast_div_num = "0";
					}
					else
					{
						cast_div_num = "";
					}
					tmmsm01["PRINT_NO"] = tmmsm01["HEAT_NO"].ToString() + tmmsm01["ST_NO"].ToString() +
						tmmsm01["STRAND_NO"].ToString() + cut_no + cast_div_num + tmmsm01["CAST_DIV_NO"].ToDecimal().Round(0).ToString();
				}
				else//非头尾坯时
				{
					if (len_no == 1)
					{
						cast_div_num = "00";
					}
					else if (len_no == 2)
					{
						cast_div_num = "0";
					}
					else
					{
						cast_div_num = "";
					}
					
					tmmsm01["PRINT_NO"] = tmmsm01["HEAT_NO"].ToString() + tmmsm01["ST_NO"].ToString() +
						tmmsm01["STRAND_NO"].ToString() + tmmsm01["MAT_NO"].ToString().Substring(8, 2) + cast_div_num +
						+tmmsm01["CAST_DIV_NO"].ToDecimal().Round(0).ToString();
				}

				
			
				Log::Info("", __FUNCTION__, "PRINT_NO=[{0}]  userid[{1}]", tmmsm01["PRINT_NO"].ToString(),s.userid);	
				Log::Info("", __FUNCTION__, "MAT_NO=[{0}] [{1}]", tmmsm01["MAT_NO"].ToString().Substring(8, 2), tmmsm01["MAT_NO"].ToString());
			}

			Log::Info("", __FUNCTION__, "tmmsm01.SLAB_NO =[{0}] MATIRAL_CODE[{1}]", tmmsm33["SLAB_NO"].ToString(), tpssm03["MATIRAL_CODE"].ToString());
			tmmsm01["SLAB_NO"] = tmmsm33["SLAB_NO"];//板坯号



			//获取计算重量    
			//首先判断钢种前两位   系数  1A 7.86  1D 7.76   1F  7.83   1M 7.83
			//若以上判断获取不到，则判断钢种第一位   1 7.85  2 7.82  3 7.82
			if (true)
			{
				CDecimal v_code_wt = 0;//计算重量的系数

				//if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 3) == "1A6")
				//{
				//	v_code_wt = 7.95;
				//}
				//else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 3) == "1A9")
				//{
				//	v_code_wt = 7.95;
				//}
				//else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 2) == "1D")
				//{
				//	v_code_wt = 7.8;
				//}
				//else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 1) == "1")
				//{
				//	v_code_wt = 7.9;
				//}
				//else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 1) == "2")
				//{
				//	v_code_wt = 7.85;
				//}
				//else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 1) == "3")
				//{
				//	v_code_wt = 7.85;
				//}

				doFlag = f_mmsm_get_density(tmmsm01["ST_NO"].ToString(), v_code_wt,conn);

				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				Log::Info("", __FUNCTION__, "v_code_wt =[{0}]", tmmsm01["ST_NO"].ToString(),v_code_wt);
				
				tmmsm01["PRODUTE_CAL_WT"] = ((tmmsm01["MAT_ACT_THICK"].ToDecimal() / 1000) * (tmmsm01["MAT_ACT_WIDTH"].ToDecimal() / 1000) * (tmmsm01["MAT_ACT_LEN"].ToDecimal() / 1000) * v_code_wt).Round(3);
				tmmsm33["PRODUTE_CAL_WT"] = tmmsm01["PRODUTE_CAL_WT"].ToDecimal().Round(3);
				tmmsm33.Update("PRODUTE_CAL_WT", "MAT_NO");
				Log::Info("", __FUNCTION__, "PRODUTE_CAL_WT =[{0}]", tmmsm01["PRODUTE_CAL_WT"].ToDecimal());
			}

			if (tmmsm01["SG_SIGN"].ToString().Trim() == "")
			{
				CString v_factory_next = "";
				CString v_cast_lot_no = "";
				if (tmmsm33["FACTORY_NEXT"].ToString().Trim() != "")
				{
					v_factory_next = tmmsm33["FACTORY_NEXT"].ToString().Trim();
				}
				if (tmmsm33["CAST_LOT_NO"].ToString().Trim() != "")
				{
					v_cast_lot_no = tmmsm33["CAST_LOT_NO"].ToString().Trim();
				}
				
				sqlstr = "SELECT SG_SIGN FROM (  "
					" SELECT row_number() over(partition by SG_SIGN order by  SLAB_NO DESC) MAX_SG, SG_SIGN  "
					" FROM TPSSM03 WHERE PONO = '"+tmmsm01["PONO"].ToString().Trim()+"' AND ORDER_NO <>' ') WHERE MAX_SG = '1'  ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmmsm01["SG_SIGN"] = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
				
				if (tmmsm01["SG_SIGN"].ToString().Trim() == "")
				{
					//4300只匹配本炉
					//其他的匹配本浇次，若都找不到，则找tpssm01表
					if (v_factory_next == "6391" || v_factory_next == "639S")
					{

					}
					else
					{
						if (v_cast_lot_no != "")
						{
							sqlstr = "SELECT SG_SIGN FROM (  "
								" SELECT row_number() over(partition by SG_SIGN order by  SLAB_NO DESC) MAX_SG, SG_SIGN  "
								" FROM TPSSM03 WHERE CAST_LOT_NO = '" + v_cast_lot_no + "' AND ORDER_NO <>' ') WHERE MAX_SG = '1'  ";
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.ExecuteReader();
							if (cmd_inq.Read())
							{
								tmmsm01["SG_SIGN"] = cmd_inq.GetString(1);
							}
							cmd_inq.Close();
						}
					}

					if (tmmsm01["SG_SIGN"].ToString().Trim() == "")
					{
						sqlstr = "SELECT SG_SIGN FROM TPSSM01 where PONO = '" + tmmsm01["PONO"].ToString().Trim() + "'";
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							tmmsm01["SG_SIGN"] = cmd_inq.GetString(1);
						}
						cmd_inq.Close();
					}

				}
				

			}


			if (tmmsm01["SLAB_NO"].ToString().Trim() == "")
			{
				tmmsm01["SLAB_NO"] = tmmsm01["PRINT_NO"];
				tmmsm33["SLAB_NO"] = tmmsm01["PRINT_NO"];//反写33表,将板坯号写进去
				tmmsm33.Update("SLAB_NO","MAT_NO");
			}
			tmmsm01["BATCH"] = tmmsm33["BATCH"];
			//获取批次号   熔炼号 +坯号   mfj   20231226 
			if (tmmsm01["BATCH"].ToString().Trim() == "")
			{
				tmmsm01["BATCH"] = tmmsm01["HEAT_NO"].ToString() + tmmsm01["SLAB_NO"].ToString().Substring(15, 2);
				
				v_batch = tmmsm01["BATCH"];
			}

			//tmmsm01["INITIAL_BATCH"] = tmmsm01["BATCH"];
			


			//modify by 178041 外卖且小工序为非生产工序才能将成品标记置为1
			//成品标记计算：有下工序则为在制品；外卖不精整的(包括修磨和二切)为成品

			////Log::Info("", __FUNCTION__, "tmmsm01["MAT_DESTION"] =[{0}]", tmmsm01["MAT_DESTION"].ToString());


			//if (tmmsm01["MAT_DESTION"].ToString() == "20")
			//{

			//	sqlstr = "SELECT BACKLOG_TYPE "
			//		"  FROM TSI0001 "
			//		"  WHERE WHOLE_BACKLOG_CODE  =  @tmmsm01.NEXT_WHOLE_BACKLOG_CODE";

			//	cmd_sql.SetCommandText(sqlstr);
			//	cmd_sql.Parameters.Clear();
			//	cmd_sql.Parameters.Set("tmmsm01.NEXT_WHOLE_BACKLOG_CODE", tmmsm01["NEXT_WHOLE_BACKLOG_CODE"].ToString());
			//	cmd_sql.ExecuteReader();
			//	if (cmd_sql.Read())
			//	{
			//		v_backlog_type = cmd_sql.GetString(1);

			//	}
			//	cmd_sql.Close();

			//	Log::Info("", __FUNCTION__, "下工序类型=[{0}]", v_backlog_type);

			//	if (v_backlog_type == "10")//下工序为生产工序
			//	{
			//		tmmsm01["PRODUCT_FLAG"] = "0";//成品标记C1 0－在制品，1－成品
			//	}
			//	else
			//	{
			//		tmmsm01["PRODUCT_FLAG"] = "1";//成品标记C1 0－在制品，1－成品
			//	}
			//}
			//else
			//{
			//余材的成品标记置空  人工在收货画面录入
			if (tmmsm01["PONO_SLAB"].ToString().Trim() == "")
			{
				tmmsm01["PRODUCT_FLAG"] = " ";//成品标记C1 0－在制品，1－成品
			}
			else
			{
				//非余材的根据去向判断   厂内是在制品，厂外是成品
				/*if (tmmsm01["MAT_DESTION"].ToString().Trim() == "60")
				{
					tmmsm01["PRODUCT_FLAG"] = "1";
				}
				else
				{
					tmmsm01["PRODUCT_FLAG"] = "0";
				}*/

				//加个不定，存在空值情况，当为空时，则成品给空  后面再确认一下 mfj  20240402
				if (tpssm03["MATIRAL_CODE"].ToString().Trim() != "")
				{
					//20240329  mfj  根据物料编码第一位来区分  F成品  H在制品
					if (tpssm03["MATIRAL_CODE"].ToString().Trim().Substring(0, 1) == "F")
					{
						tmmsm01["PRODUCT_FLAG"] = "1";
					}
					else if (tpssm03["MATIRAL_CODE"].ToString().Trim().Substring(0, 1) == "H")
					{
						tmmsm01["PRODUCT_FLAG"] = "0";
					}
				}
				else
				{
					tmmsm01["PRODUCT_FLAG"] = " ";
				}
				
			}


				
			//}

			////Log::Info("", __FUNCTION__, "tmmsm01["MAT_SHAPE_FLAG"] =[{0}]", tmmsm01["MAT_SHAPE_FLAG"].ToString());
			////Log::Info("", __FUNCTION__, "tmmsm01["ORDER_NO"] =[{0}]", tmmsm01["ORDER_NO"].ToString());

			//------------------获取实物材料的合同信息--------------------------

			//if (tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() == "2" && tmmsm01["ORDER_NO"].ToString().Trim() != "")
			//{

			//	switch (conn->DatabaseKind)
			//	{
			//	case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//	case DB_KIND_ORACLE:        // Oracle 数据库
			//	default:
			//		//合同类型
			//		sqlstr = CString("select merg_type from tpmof01 where order_no = @order_no");

			//		break;
			//	}
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.Parameters.Set("order_no", tmmsm01["ORDER_NO"].ToString());
			//	cmd_inq.ExecuteReader();
			//	if (cmd_inq.Read())
			//	{
			//		tmmsm01["COMBINE_FLAG"] = cmd_inq.GetString(1).Substring(0, 1);
			//	}
			//	else
			//	{
			//		tmmsm01["COMBINE_FLAG"] = " ";
			//	}
			//	cmd_inq.Close();

			//	Log::Trace("", __FUNCTION__, "tmmsm01[\"ORDER_NO\"] =[{0}], tmmsm01[\"COMBINE_FLAG\"] =[{1}]", tmmsm01["ORDER_NO"].ToString(), tmmsm01["COMBINE_FLAG"].ToString());


			//}


			Log::Info("", __FUNCTION__, "aaaaaaaaaaaaaa", tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"].ToDecimal());

			if (!bcls_rec->Tables["TMMSM33_IC"].Columns.Contains("FIN_ST_NO"))
			{
				bcls_rec->Tables["TMMSM33_IC"].Columns.Add(DT_STRING, "FIN_ST_NO");
			}
			tmmsm01["FIN_ST_NO"] = tmmsm33["FIN_ST_NO"];
			tmmsm01["HOLD_FLAG"] = "0";


#if defined _SYS_PES || defined _SYS_MES 
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:        // Oracle 数据库
			default:
				//合同类型
				sqlstr = CString("SELECT FIN_ST_NO,JUDGE_CODE FROM TQMTS23 WHERE HEAT_NO = @heat_no ");

				break;
			}
#endif

#if defined _SYS_MMS
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:        // Oracle 数据库
			default:
				//合同类型
				sqlstr = CString("SELECT ST_NO,PCH_JUDGE_CODE FROM TQMTQB0 WHERE HEAT_NO = @heat_no ");

				break;
			}

#endif

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tmmsm01["HEAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				v_fin_st_no = cmd_inq.GetString(1);
				v_judge_code = cmd_inq.GetString(2);

				//如果是交接坯,不置最终出钢记号
				if (tmmsm01["TRANSFER_FLAG"].ToString().Trim() != "1")
				{
					if (v_fin_st_no.Trim() != "")
					{
						tmmsm01["FIN_ST_NO"] = cmd_inq.GetString(1);
						tmmsm01["ST_NO"] = cmd_inq.GetString(1);
					}
					else if (v_judge_code.Trim() == "2")//不合格 质量封锁       太钢定制  因所有判定操作都在产销  三级取消掉自动封锁
					{
						//tmmsm01["HOLD_FLAG"] = "2";
					}
				}
			}

			cmd_inq.Close();





			if (tmmsm01["SURFACE_DECIDE_CODE"].ToString().Trim() == "2" || tmmsm01["IF_TRANSFER"].ToString().Trim() == "1")
			{
				//表面不合或交接坯,质量封锁
				//tmmsm01["HOLD_FLAG"] = "2";
			}

			//头尾坯

			Log::Trace("", __FUNCTION__, "heat_No=[{0}], tmmsm01[\"FIN_ST_NO\"] =[{1}],tmmsm01[\"HOLD_FLAG\"] =[{2}]", tmmsm01["HEAT_NO"].ToString(), tmmsm01["FIN_ST_NO"].ToString(), tmmsm01["HOLD_FLAG"].ToString());

			bcls_rec->Tables["TMMSM33_IC"].Rows[i]["FIN_ST_NO"] = tmmsm01["FIN_ST_NO"];


			if (tmmsm01["FIN_ST_NO"].ToString().Trim() != "")
			{
				tmmsm01["ST_NO"] = tmmsm01["FIN_ST_NO"];
			}

			doFlag = f_mmsm_get_theorywt(tmmsm01["MAT_ACT_THICK"].ToDecimal(), tmmsm01["MAT_ACT_WIDTH"].ToDecimal(), tmmsm01["MAT_ACT_LEN"].ToDecimal(), tmmsm01["MAT_NUM"].ToDecimal(), MAT_THEORY_WT);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", "", "理论重量tmmsm01[\"MAT_THEORY_WT\"] = {0}", tmmsm01["MAT_THEORY_WT"].ToDecimal());
			tmmsm01["MAT_THEORY_WT"] = MAT_THEORY_WT;
			//增加MAT_WT 重量赋值
			if (tmmsm01["MAT_ACT_WT"].ToDecimal() > 0)
			{
				tmmsm01["MAT_WT"] = tmmsm01["MAT_ACT_WT"].ToDecimal().Round(3);
			}
			else
			{
				tmmsm01["MAT_WT"] = tmmsm01["L3_CALTHEROY_WT"].ToDecimal().Round(3);
			}

			//钢坯产出后,获取试批号
			//f_mmsm_sample_lot_no(bcls_rec, tmmsm01, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			Log::Trace("", "", "tmmsm01[\"HOT_SEND_FLAG\"] = {0}", tmmsm01["HOT_SEND_FLAG"].ToString());


			//应舞阳要求，增加需要判断是否精整、缓冷和退火

			if (tmmsm01["HOT_SEND_FLAG"].ToString() == "0")
			{ //如果是冷送 直接置精整标记 判断热处理
				//缓冷、退火
				tqmts08["SLAB_COOL_IND"] = " ";
				cmd_inq.SetCommandText("SELECT SLAB_COOL_IND,MACH_CLEAR_DIV FROM TQMTS08 WHERE ST_NO = @st_no ORDER BY VERSION DESC ");
				cmd_inq.Parameters.Set("st_no", tmmsm01["ST_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmts08["SLAB_COOL_IND"] = cmd_inq.GetString(1);
					tmmsm01["FINISH_FLAG"] = cmd_inq.GetString(2);
				}
				cmd_inq.Close();
				if (tqmts08["SLAB_COOL_IND"].ToString() == "1"){
					tmmsm01["COOL_FLAG"] = "1";
				}
				else if (tqmts08["SLAB_COOL_IND"].ToString() == "2"){
					tmmsm01["ANNEAL_FLAG"] = "1";
				}

				Log::Trace("", "", "tqmts08[\"SLAB_COOL_IND\"] = {0}", tqmts08["SLAB_COOL_IND"].ToString());
				Log::Trace("", "", "tmmsm01[\"COOL_FLAG\"] = {0}", tmmsm01["COOL_FLAG"].ToString());
				Log::Trace("", "", "tmmsm01[\"ANNEAL_FLAG\"] = {0}", tmmsm01["ANNEAL_FLAG"].ToString());

			}

			if (tmmsm33["STATION_ID"].ToString() == "I") //模铸必须脱模
			{
				tmmsm01["EJECTION_FLAG"] = "1";
			}


			tmmsm01["IC_CC_FLAG"] = tmmsm33["STATION_ID"];  //模连区分

			
			//获取碳锈区分标记   mfj  20231127    添加钢种描述  mfj  20240320 
			CString sql_tqmts0x = "SELECT C_DIV,SG_GRADE_1 FROM TQMTS0X WHERE ST_NO = '" + tmmsm01["ST_NO"].ToString() + "'";
			cmd_inq_tqmts0x.SetCommandText(sql_tqmts0x);
			cmd_inq_tqmts0x.ExecuteReader();
			if (cmd_inq_tqmts0x.Read())
			{
				tqmts0x["C_DIV"] = cmd_inq_tqmts0x.GetString(1);
				tqmts0x["SG_GRADE_1"] = cmd_inq_tqmts0x.GetString(2);
			}
			cmd_inq_tqmts0x.Close();
			

			if (tmmsm01["C_DIV"].ToString().Trim() == "")
			{
				tmmsm01["C_DIV"] = tqmts0x["C_DIV"].ToString().Trim();
			}

			//将不为12的碳锈区分，改为12
			if (tmmsm01["C_DIV"].ToString().Trim() != "")
			{
				if (tmmsm01["C_DIV"].ToString().Trim() == "4")
				{
					tmmsm01["C_DIV"] = "1";
				}

				if (tmmsm01["C_DIV"].ToString().Trim() == "3" || tmmsm01["C_DIV"].ToString().Trim() == "5")
				{
					tmmsm01["C_DIV"] = "2";
				}
			}
			
			//存在工艺卡没有该钢种数据情况，加个判断，若一直存在这种情况，则需要考虑在接收程序里添加其他获取方式   mfj  20240415
			if (tmmsm01["SG_GRADE_1"].ToString().Trim() == "")
			{
				tmmsm01["SG_GRADE_1"] = tqmts0x["SG_GRADE_1"].ToString().Trim();
			}
			
			CString steel = tmmsm01["C_DIV"].ToString() == "1" ? "S" : "C";
			tmmsm01["STEEL_GROUP"] = steel + tmmsm01["ST_NO"].ToString();//--Lz钢种组

			//获取设备名称    mfj  20231127 
			cmd_inq.SetCommandText("SELECT STATION_NAME FROM TPSSMD1 WHERE DEV_CODE ='"+tmmsm01["DEV_CODE"].ToString()+"'");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm01["DEV_CNAME"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();

			//获取连铸初判数据，获取不到赋默认值A
			cmd_inq.SetCommandText("SELECT CK_RESULT from tmmsm3f where slab_no = '"+ tmmsm01["SLAB_NO"].ToString().Trim() +"'  ORDER BY  ID DESC");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm01["CASTING_PRE_JUDGMENT"] = cmd_inq.GetString(1);
			}
			else//没有获取到时给默认值
			{
				tmmsm01["CASTING_PRE_JUDGMENT"] ="A";
			}
			cmd_inq.Close();

			//cmd_inq.SetCommandText("SELECT BACK_N_1,BACK_N_2,BACK_C1,BACK_C2 FROM TWMSMPZ WHERE CODE_CLASS='JHKD' AND CODE_DESC_1_CONTENT='" + tmmsm01["STRAND_NO"].ToString() + "'");
			//cmd_inq.ExecuteReader();
			//if (cmd_inq.Read())
			//{
			//	if (cmd_inq.GetString(3)=="1")
			//	{
			//		if (cmd_inq.GetString(4).Trim() != "")//三级浇次号存在，判断三级浇次号和宽度偏差
			//		{
			//			if (tmmsm01["MAT_WIDTH"].ToDecimal() + cmd_inq.GetDecimal(2) == cmd_inq.GetDecimal(1) && tmmsm01["CAST_NO"].ToString() == cmd_inq.GetString(4))
			//			{
			//				tmmsm01["MAT_WIDTH"] = cmd_inq.GetDecimal(1);
			//				tmmsm01["MAT_ACT_WIDTH"] = cmd_inq.GetDecimal(1);
			//			}
			//			else {
			//				twmsmpz["CODE_CLASS"] = "JHKD";
			//				twmsmpz["CODE_DESC_1_CONTENT"] = tmmsm01["STRAND_NO"].ToString();
			//				twmsmpz["BACK_C1"] = "0";
			//				twmsmpz["BACK_C2"] = " ";
			//				twmsmpz.Update("BACK_C1,BACK_C2", "CODE_CLASS,CODE_DESC_1_CONTENT");
			//			}
			//		}
			//		else {//三级浇次号不存在，只判断宽度偏差
			//			if (tmmsm01["MAT_WIDTH"].ToDecimal() + cmd_inq.GetDecimal(2) == cmd_inq.GetDecimal(1))
			//			{
			//				tmmsm01["MAT_WIDTH"] = cmd_inq.GetDecimal(1);
			//				tmmsm01["MAT_ACT_WIDTH"] = cmd_inq.GetDecimal(1);

			//				twmsmpz["CODE_CLASS"] = "JHKD";
			//				twmsmpz["CODE_DESC_1_CONTENT"] = tmmsm01["STRAND_NO"].ToString();
			//				twmsmpz["BACK_C2"] = tmmsm01["CAST_NO"];
			//				twmsmpz.Update("BACK_C2", "CODE_CLASS,CODE_DESC_1_CONTENT");
			//			}
			//			else {
			//				twmsmpz["CODE_CLASS"] = "JHKD";
			//				twmsmpz["CODE_DESC_1_CONTENT"] = tmmsm01["STRAND_NO"].ToString();
			//				twmsmpz["BACK_C1"] = "0";
			//				twmsmpz["BACK_C2"] = " ";
			//				twmsmpz.Update("BACK_C1,BACK_C2", "CODE_CLASS,CODE_DESC_1_CONTENT");
			//			}
			//		}
			//	} 
			//}
			//cmd_inq.Close();

			tmmsm01["SYS_CODE"] = v_sys_code;
			tmmsm01.TrimOrBlank();
			//tmmsm01["PILE_INDEX"] = " ";
			//Log::Trace("", "", "insert tmmsm01.TrimOrBlank");
			//Log::Trace("", "", "insert tmmsm01["PILE_INDEX"] = [{0}] ", tmmsm01["PILE_INDEX"].ToString());
			////Log::Info("", __FUNCTION__, "tmmsm01["MAT_NO"] =[{0}],tmmsm01["ORDER_NO"] =[{1}]", tmmsm01["MAT_NO"].ToString(), tmmsm01["ORDER_NO"].ToString());//合同号

			//===============生成主档=================================
			//物料主档记录插入 modify by xiangping  2018/5/23

			//tmmsm01.Print();

			/*	sqlstr = "tmmsm01.Insert()";
			tmmsm01.TrimOrBlank();
			if (tmmsm01.Insert() == false)
			{
			tmmsm01.Print();
			}*/
			//========================================================




			/* 设置物料跟踪参数 */
			//bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
			//bcls_rec->Tables["MM0099"].Rows[i]["EVENT_ID"] = "MM09";
			//bcls_rec->Tables["MM0099"].Rows[i]["EVENT_LINE_TYPE"] = "00";
			//bcls_rec->Tables["MM0099"].Rows[i]["SYSTEM_ID"] = "MMSM";
			//bcls_rec->Tables["MM0099"].Rows[i]["FUNC_ID"] = "f_mmsm01_ins";
			//bcls_rec->Tables["MM0099"].Rows[i]["MAT_NO"] = tmmsm01["MAT_NO"];
			//bcls_rec->Tables["MM0099"].Rows[i]["EVENT_DESC"] = "板坯切断";
		


			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "MM09";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = "f_mmsm01_ins";
			tmmsm96["EVENT_DESC"] = "工序[" + tmmsm01["WHOLE_BACKLOG_CODE"].ToString() + "]产出";
			//bcls_rec->Tables["MM0099"].Clear();
			tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);


			//Log::Trace("", __FUNCTION__, "_LINE_HP_tmmsm01["MAT_NO"] =[{0}], ", tmmsm01["MAT_NO"].ToString());

			//Log::Trace("", __FUNCTION__, "v_slab_dest_code111=[{0}], ", v_slab_dest_code);


#if defined _LINE_HP
			//厚板特殊调用START
			//if(tmmsm01["MAT_DESTION"].ToString().Trim() == "10")

			//Log::Trace("", __FUNCTION__, "v_slab_dest_code222=[{0}], ", v_slab_dest_code);

			if (v_slab_dest_code == "HP")
			{
				//================创建目的板坯档函数=====================

				tmmsm01.MergeTo(bcls_rec->Tables["MMHP0001"], false);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
#endif


#if defined _SYS_MMS || defined _SYS_MES 
			/*调用 物料跟踪路径新增 */
			tmm0005["MAT_NO"] = tmmsm01["MAT_NO"];
			tmm0005["MAT_KIND"] = tmmsm01["MAT_KIND"];
			tmm0005["MAT_PROD_FLAG"] = "20";		//材料产出标记 10-原料录入;20-机组产出;21-并卷;22-分卷;30-整卷回退;31-半卷回退;50-清盘库
			tmm0005["MAIN_MAT_FLAG"] = "1";		//主材料标记  0-非主材料,1-主材料
			tmm0005["SPECAIL_FLAG"] = "0";		//特殊标记 0-默认值,1-并卷同时分卷
			tmm0005.MergeTo(bcls_rec->Tables["MM000501"], false);
#endif


			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


			/*add by xp 2014/08/26 调用组卷信息更新*/

			//#if defined _LINE_CR
			//if (v_order_type.Trim() == "A")
			//{

			//	bcls_rec->Tables["PMOU"].Clear();
			//	bcls_rec->Tables["PMOU"].Columns.Add(DT_STRING, "MAT_KIND");
			//	bcls_rec->Tables["PMOU"].Columns.Add(DT_STRING, "MAT_NO");
			//	bcls_rec->Tables["PMOU"].Rows.Add(); // 创建一行

			//	bcls_rec->Tables["PMOU"].Rows[0]["MAT_KIND"] = tmmsm01["MAT_KIND"];
			//	bcls_rec->Tables["PMOU"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];


			//	//doFlag = f_pmoucr_mm_upd(bcls_rec, bcls_ret, conn);
			//	if (doFlag < 0)
			//	{
			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//	}
			//}
			//#endif

			//2023/9/15 鉴于方坯批量产出，L3仅封锁最后一块，L4全部封锁。故把赋值放在循环内
			sqlstr =
				" SELECT sum(SLAB_MIN_LEN),sum(SLAB_MAX_LEN) from tpssm03 where lslab_no =@lslab_no  ";

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("lslab_no", tpssm03["LSLAB_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tpssm03["SLAB_MIN_LEN"] = cmd_inq.GetDecimal(1);
				tpssm03["SLAB_MAX_LEN"] = cmd_inq.GetDecimal(2);
			}
			cmd_inq.Close();

			Log::Trace("", __FUNCTION__, "SLAB_MIN_LEN =[{0}], ", tpssm03["SLAB_MIN_LEN"].ToString());
			Log::Trace("", __FUNCTION__, "SLAB_MAX_LEN =[{0}], ", tpssm03["SLAB_MAX_LEN"].ToString());
			Log::Trace("", __FUNCTION__, "SLAB_NUM =[{0}], ", tpssm03["SLAB_NUM"].ToString());
			Log::Trace("", __FUNCTION__, "MAT_ACT_LEN =[{0}], ", tmmsm01["MAT_ACT_LEN"].ToString());
			Log::Trace("", __FUNCTION__, "LSLAB_NO =[{0}], ", tpssm03["LSLAB_NO"].ToString());

			//20240104  太钢定制  所有判定在四级，三级不自动封锁。
			/*if (tmmsm01["MAT_ACT_LEN"].ToDecimal() < tpssm03["SLAB_MIN_LEN"].ToDecimal() || tmmsm01["MAT_ACT_LEN"].ToDecimal() > tpssm03["SLAB_MAX_LEN"].ToDecimal())
			{
				Log::Trace("", __FUNCTION__, "进来啦=[{0}], ", tmmsm01["MAT_ACT_LEN"].ToString());
				bcls_rec_QM17.Tables[0].Rows.Clear();
				bcls_rec_QM17.Tables[0].Rows.Add();
				bcls_rec_QM17.Tables[0].Rows[i]["EVENT_ID"] = "QM17";
				bcls_rec_QM17.Tables[0].Rows[i]["EVENT_LINE_TYPE"] = "00";
				bcls_rec_QM17.Tables[0].Rows[i]["SYSTEM_ID"] = "MMSM";
				bcls_rec_QM17.Tables[0].Rows[i]["FUNC_ID"] = "f_mmmsm01_ins";
				bcls_rec_QM17.Tables[0].Rows[i]["MAT_NO"] = tmmsm01["MAT_NO"];
				bcls_rec_QM17.Tables[0].Rows[i]["REL_REMARK"] = "炼钢长度不合封锁";
				bcls_rec_QM17.Tables[0].Rows[i]["REL_MAKER"] = s.userid;
				bcls_rec_QM17.Tables[0].Rows[i]["REL_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				bcls_rec_QM17.Tables[0].Rows[i]["HOLD_CAUSE_CODE"] = "QMZ7";
				bcls_rec_QM17.Tables[0].Rows[i]["DEFECT_CLASS"] = " ";
			}*/

		}

		//Log::Trace("", "", "MMSM99START");

		//Log::Trace("", __FUNCTION__, "99行数=[{0}], ", bcls_rec->Tables["MM0099"].Rows.get_Count());

		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
		{
			CString v_mat_no = "";
			CString v_order_no = "";
			for (int mm99_row = 0; mm99_row < bcls_rec->Tables["MM0099"].Rows.get_Count(); mm99_row++)
			{
				v_mat_no = bcls_rec->Tables["MM0099"].Rows[mm99_row]["MAT_NO"].ToString();
				v_order_no = bcls_rec->Tables["MM0099"].Rows[mm99_row]["ORDER_NO"].ToString();
				//Log::Trace("", __FUNCTION__, "mm99_row=[{0}], ", mm99_row);
				//Log::Trace("", __FUNCTION__, "_LINE_HP_tmmsm01["MAT_NO"] =[{0}], ", v_mat_no);
				//Log::Trace("", __FUNCTION__, "_LINE_HP_tmmsm01["ORDER_NO"] =[{0}], ", v_order_no);
			}

			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		
		//2023/9/15 鉴于方坯批量产出，L3仅封锁最后一块，L4全部封锁。故把赋值放在循环内
		//sqlstr =
		//	" SELECT sum(SLAB_MIN_LEN),sum(SLAB_MAX_LEN) from tpssm03 where lslab_no =@lslab_no  ";

		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("lslab_no", tpssm03["LSLAB_NO"].ToString());
		//cmd_inq.ExecuteReader();
		//if (cmd_inq.Read())
		//{
		//	tpssm03["SLAB_MIN_LEN"] = cmd_inq.GetDecimal(1);
		//	tpssm03["SLAB_MAX_LEN"] = cmd_inq.GetDecimal(2);
		//}
		//cmd_inq.Close();

		//Log::Trace("", __FUNCTION__, "SLAB_MIN_LEN =[{0}], ", tpssm03["SLAB_MIN_LEN"].ToString());
		//Log::Trace("", __FUNCTION__, "SLAB_MAX_LEN =[{0}], ", tpssm03["SLAB_MAX_LEN"].ToString());
		//Log::Trace("", __FUNCTION__, "SLAB_NUM =[{0}], ", tpssm03["SLAB_NUM"].ToString());
		//Log::Trace("", __FUNCTION__, "MAT_ACT_LEN =[{0}], ", tmmsm01["MAT_ACT_LEN"].ToString());

		//if (tmmsm01["MAT_ACT_LEN"].ToDecimal() < tpssm03["SLAB_MIN_LEN"].ToDecimal() || tmmsm01["MAT_ACT_LEN"].ToDecimal() > tpssm03["SLAB_MAX_LEN"].ToDecimal())
		//{
		//	bcls_rec_QM17.Tables[0].Rows.Clear();
		//	bcls_rec_QM17.Tables[0].Rows.Add();
		//	bcls_rec_QM17.Tables[0].Rows[0]["EVENT_ID"] = "QM17";
		//	bcls_rec_QM17.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
		//	bcls_rec_QM17.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
		//	bcls_rec_QM17.Tables[0].Rows[0]["FUNC_ID"] = "f_mmmsm01_ins";
		//	bcls_rec_QM17.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
		//	bcls_rec_QM17.Tables[0].Rows[0]["REL_REMARK"] = "炼钢长度不合封锁";
		//	bcls_rec_QM17.Tables[0].Rows[0]["REL_MAKER"] = s.userid;
		//	bcls_rec_QM17.Tables[0].Rows[0]["REL_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//	bcls_rec_QM17.Tables[0].Rows[0]["HOLD_CAUSE_CODE"] = "QMZ7";
		//	bcls_rec_QM17.Tables[0].Rows[0]["DEFECT_CLASS"] = " ";

			//若存在由于长度不合要求，则调此函数封锁	
		Log::Trace("", __FUNCTION__, "11111SLAB_MIN_LEN =[{0}], ", tpssm03["SLAB_MIN_LEN"].ToString());
		if (bcls_rec_QM17.Tables[0].Rows.get_Count() > 0)
		{
			//20240104  太钢定制  所有判定在四级，三级不自动封锁。
			//doFlag = f_mmsm99(&bcls_rec_QM17, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}	

		//}
	

		//Log::Trace("", "", "MMSM99END");


		//Log::Trace("", "", "MMHP0001START");
		//Log::Trace("", __FUNCTION__, "MMHP0001行数111=[{0}], ", bcls_rec->Tables["MMHP0001"].Rows.get_Count());

		#if defined _LINE_HP
		//Log::Trace("", __FUNCTION__, "MMHP0001行数222=[{0}], ", bcls_rec->Tables["MMHP0001"].Rows.get_Count());

		if (bcls_rec->Tables["MMHP0001"].Rows.get_Count() > 0)
		{
			doFlag = f_mmhp0001_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		Log::Trace("", __FUNCTION__, "tmmsm01.NEXT_SUB_BACKLOG_CODE =[{0}], ", tmmsm01["NEXT_SUB_BACKLOG_CODE"].ToString());
		if (tmmsm01["HOT_CHARGE_FLAG"].ToString() == "2" && tmmsm01["MAT_DESTION"].ToString() == "18" && tmmsm01["PONO_SLAB_2"].ToString().Trim() == "")
		{

			bcls_rec_PSHP.Tables[0].Rows.Clear();
			bcls_rec_PSHP.Tables[0].Rows.Add();
			bcls_rec_PSHP.Tables[0].Rows[0]["OP_TYPE"] = "3";
			bcls_rec_PSHP.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];

			doFlag = f_pshp_top_trace(&bcls_rec_PSHP, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

		}
		#endif

        #if defined _SYS_MMS || defined _SYS_MES 
		//if (bcls_rec->Tables["MM000501"].Rows.get_Count() > 0)
		//{
		//	doFlag = f_mm000501_proc(bcls_rec, bcls_ret, conn);
		//	if (doFlag < 0)
		//	{
		//		doFlag = 0;
		//		s.flag = 0;
		//		//throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
		//}
        #endif

		//从主档获取小工序要在f_mmhp0001_proc函数后调用
		


	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;

}


