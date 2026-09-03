/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2016-09-01
Description: 炼钢钢坯材料信息修改
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢钢坯材料信息修改
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/
/******
修改时 ，系统重量和收货重量一致才可以改
修改磨后重量时，校验当前的系统重量和磨后重量一致
当修改磨后量时，有修磨标记，且长宽厚重一致的情况下，且没有分切的时候，允许修改。其他情况则在修磨实绩画面修改磨后量
修改时，必须是综判取消，没有修磨标记的，则修改磨前量

********/


//业务头文件


#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES

#endif

//外部函数声明
//计算理重
BM2_FUNCTION_EXPORT
int f_mm0012(CDecimal w_length,			/* Length		    (mm)    */
CDecimal w_width,						/* Width		    (mm)	*/
CDecimal w_thick,						/* Thickness	    (mm)	*/
CDecimal w_density,						/* Material density (g/cm3) */
CDecimal w_ctwg,						/* Coating weight   (g/m2)  */
CDecimal& w_weight,						/* Weight 		    (t)		*/
CDbConnection * conn);

BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
int f_wmsm_t8p301_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天
int f_mmsm_e2t8m1_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_e2t8m1_miss(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm_get_density(CString ST_NO, CDecimal& MAT_DENSITY, CDbConnection* conn);//通过钢种计算密度
int f_t8z_23m_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//发送专家系统数据

#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
BM2_FUNCTION_IMPORT
int f_pmof99_v3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

BM2F_ENTERACE(mmsm01a1f4_upd)

int f_mmsm01a1f4_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CDecimal w_density = 7.85;	/* Material density (g/cm3) */
	CDecimal w_ctwg = 0;		/* Coating weight   (g/m2)  */
	CDecimal MAT_THEORY_WT = 0;
	CString v_button_check = "";//按钮区分
	CDecimal v_upd_flag = 0;//是否更新03表数据  1  更新， 其他情况不更新---只有未炉次关闭的才更新
	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel hmmsm01("HMMSM01");
	CModel tmmsm01("TMMSM01");
	CModel para_tmmsm01("TMMSM01");//弹窗数据
	CModel tmmsm01_ysjl("TMMSM01");//原始记录
	CModel tmmsm01_t8ps02("TMMSM01");//电文专用
	CModel tmmsm96("TMMSM96");
	CModel tpssm03("TPSSM03");

#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
	CModel tpmof03("TPMOF03");
#endif

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq_pono(conn);

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "SURF_QUALITY");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");

	EIClass inblock;
	inblock.Tables[0].Columns.Add(tmmsm01);
	inblock.Tables[0].Rows.Clear();

	EIClass in_23m;
	in_23m.Tables[0].Columns.Add(DT_STRING, "TC_NO");
	in_23m.Tables[0].Rows.Add();
	in_23m.Tables[0].Rows[0]["TC_NO"] = "T82322";
	in_23m.Tables.Add();
	in_23m.Tables[1].Columns.Add(tmmsm01);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 添加与设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
			bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
		}

#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
		blkNum = bcls_rec->Tables.IndexOf("PMOF99");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("PMOF99");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "event_id");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "order_no");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "system_id");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "func_id");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "whole_backlog");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "whole_backlog_seq");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "whole_backlog_code");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "mat_no");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "mat_status");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "wt");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "prev_mat_no");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "prev_wt");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "keyvalue_1");
			bcls_rec->Tables["PMOF99"].Columns.Add(DT_STRING, "keyvalue_2");
		}
#endif


		/*获得传入参数*/
		para_tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		para_tmmsm01.TrimOrBlank();
		hmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		tmmsm01["MAT_NO"] = para_tmmsm01["MAT_NO"];
		tmmsm01.Query("MAT_NO");
		tmmsm01.TrimOrBlank();
		tmmsm01_t8ps02.CopyFrom(tmmsm01);
		//Log::Trace("",__FUNCTION__,"para_tmmsm01.MAT_NO		= [{0}]",(const char*)para_tmmsm01["MAT_NO"].ToString());


		if (bcls_rec->Tables[0].Columns.Contains("Button_check"))
			v_button_check = bcls_rec->Tables[0].Rows[0]["Button_check"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "v_button_check= [{0}]", (const char*)v_button_check);

		/* 检查输入参数合法性 */
		if (para_tmmsm01["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "材料号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (hmmsm01.QueryCount("MAT_NO") > 0)
		{
			strcpy(s.msg, "材料已出库，不可进行修改操作！");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (para_tmmsm01["MAT_THICK"].ToDecimal() <= 0)
		{
			sprintf(s.msg, "材料号[%s]厚度[%f]不能小于0!", (const char*)para_tmmsm01["MAT_NO"].ToString(), para_tmmsm01["MAT_THICK"].ToDecimal().ToDouble());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (para_tmmsm01["MAT_WIDTH"].ToDecimal() <= 0)
		{
			sprintf(s.msg, "材料号[%s]宽度[%f]不能小于0!", (const char*)para_tmmsm01["MAT_NO"].ToString(), para_tmmsm01["MAT_WIDTH"].ToDecimal().ToDouble());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (para_tmmsm01["MAT_LEN"].ToDecimal() <= 0)
		{
			sprintf(s.msg, "材料号[%s]长度[%d]不能小于0!", (const char*)para_tmmsm01["MAT_NO"].ToString(), para_tmmsm01["MAT_LEN"].ToDecimal().ToInt32());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (para_tmmsm01["MAT_WT"].ToDecimal() <= 0)
		{
			strcpy(s.msg, "材料重量不能为0");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (para_tmmsm01["MAT_NUM"].ToDecimal() <= 0)
		{
			sprintf(s.msg, "材料号[%s]钢板数[%d]不能小于0!", (const char*)para_tmmsm01["MAT_NO"].ToString(), para_tmmsm01["MAT_NUM"].ToDecimal().ToInt32());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

	
		//添加对应按钮校验 
		//F4按钮  必须收货后才可改    修改重量规格尺寸
		if (v_button_check == "F4_BUTTON")
		{
			if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
			{
				sprintf(s.msg, "材料[%s]未收货，不能修改钢种炉号和材料号，请先进行收货撤销!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (para_tmmsm01["LGORT"].ToString() == "6246" && (bcls_rec->Tables["YSJL"].Rows[0]["MAT_ACT_WT"] != para_tmmsm01["MAT_WT"].ToDecimal()))
			{
				strcpy(s.msg, "材料当前存货地为6246，不可进行修改操作");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (para_tmmsm01["COMPLEX_DECIDE_CODE"].ToString() != "0")
			{
				strcpy(s.msg, "材料已综判，不可进行修改操作");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (para_tmmsm01["USAGE_DECISION"].ToString() == "3001")
			{
				strcpy(s.msg, "材料已综判合格，不可进行修改操作");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (para_tmmsm01["MEND_FLAG"].ToString().Trim() != "0"  && para_tmmsm01["MEND_FLAG"].ToString().Trim() != "" && (bcls_rec->Tables["YSJL"].Rows[0]["MAT_ACT_WT"] != para_tmmsm01["MAT_WT"].ToDecimal()))
			{
				strcpy(s.msg, "材料已做修磨，不可进行修改操作!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			CString rcv_date_month = Db::QueryCString("select RECV_MAT_TIME from Tmmsm01 where MAT_NO='" + tmmsm01["MAT_NO"].ToString() + "'");
		
			Log::Trace("", __FUNCTION__, "RCV_date_month	= [{0}]", rcv_date_month);
			if (rcv_date_month.Trim() != "" && rcv_date_month.SubstringNE(0, 6) < datetime.SubstringNE(0, 6) && para_tmmsm01["MAT_WT"].ToDecimal()!=tmmsm01["MAT_WT"].ToDecimal())
			{
				strcpy(s.msg, "跨月收货的板坯，不可修改材料重量(t)!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		//F7按钮 必须未收货 修改钢种，炉号
		if (v_button_check == "F7_BUTTON")
		{
			
			if (para_tmmsm01["LGORT"].ToString() == "6246")
			{
				strcpy(s.msg, "材料当前存货地为6246，不可进行修改操作");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (para_tmmsm01["COMPLEX_DECIDE_CODE"].ToString() != "0")
			{
				strcpy(s.msg, "材料已综判，不可进行修改操作");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (para_tmmsm01["USAGE_DECISION"].ToString() == "3001")
			{
				strcpy(s.msg, "材料已综判合格，不可进行修改操作");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (para_tmmsm01["MEND_FLAG"].ToString().Trim() != "0"  && para_tmmsm01["MEND_FLAG"].ToString().Trim() != "")
			{
				strcpy(s.msg, "材料已做修磨，不可进行修改操作!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (para_tmmsm01["MAT_NO"].ToString().SubstringNE(0,8)!= para_tmmsm01["SLAB_NO"].ToString().SubstringNE(0, 8) 
				|| para_tmmsm01["ST_NO"].ToString()!= para_tmmsm01["SLAB_NO"].ToString().SubstringNE(8, 6)
				|| para_tmmsm01["MAT_NO"].ToString().SubstringNE(8, 2) != para_tmmsm01["SLAB_NO"].ToString().SubstringNE(15, 2))
			{
				strcpy(s.msg, "板坯号不符，请点击空白区域重新计算板坯号!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		if (v_button_check == "F7_BUTTON")
		{
			EIClass miss_E2T8M1;
			miss_E2T8M1.Tables[0].set_TableName("E2T8M1");
			miss_E2T8M1.Tables["E2T8M1"].Columns.Add(DT_STRING, "MAT_NO");
			miss_E2T8M1.Tables["E2T8M1"].Rows.Add();
			miss_E2T8M1.Tables["E2T8M1"].Rows[0]["MAT_NO"] = bcls_rec->Tables["YSJL"].Rows[0]["MAT_NO"].ToString();

			doFlag = f_wmsm_e2t8m1_miss(&miss_E2T8M1, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		

		if (!bcls_rec->Tables.Contains("YSJL"))
		{
			strcpy(s.msg, "YSJL块数据不存在，请联系开发人员!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		EIClass bcls_rec_WM02;
		CString v_bmzl = "";
		if (para_tmmsm01["STOCK_L2"].ToString().Trim() == "")
		{
			strcpy(s.msg, "二级库存地不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		CString c_quxiang = Db::QueryCString(" select CODE_DESC_1_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE =(select GUIDE_DEST from tmmsm01 where MAT_NO='" + para_tmmsm01["MAT_NO"].ToString() + "') ");
		if (c_quxiang.Find("2250") >= 0&& bcls_rec->Tables[0].Rows[0]["STOCK_L2"].ToString() != bcls_rec->Tables["YSJL"].Rows[0]["STOCK_L2"].ToString())
		{
			strcpy(s.msg, "只有非2250去向，才允许修改二级库存地!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		sqlstr = " SELECT CODE_DESC_1_CONTENT FROM TWMSMZD02 WHERE  CODE_CLASS ='MMBMZL' and code='" + para_tmmsm01["SURF_QUALITY"].ToString() + "' ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			v_bmzl = cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		if (v_bmzl.Find("调宽") >= 0)
		{
			para_tmmsm01["ADJUST_WIDTH_MARK"] = "1";
		}
		else
		{
			para_tmmsm01["ADJUST_WIDTH_MARK"] = " ";
		}


		//熔炼号校验
		if (para_tmmsm01["HEAT_NO"].ToString().Substring(0, 1) != "A" && para_tmmsm01["HEAT_NO"].ToString().Substring(0, 1) != "B")
		{
			strcpy(s.msg, "熔炼号规则第一位必须是A或B，请重新确认！");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		//获取计算重量    
		//首先判断钢种前两位   系数  1A 7.86  1D 7.76   1F  7.83   1M 7.83
		//若以上判断获取不到，则判断钢种第一位   1 7.85  2 7.82  3 7.82
		if (true)
		{
			CDecimal v_code_wt = 0;//计算重量的系数

			/*if (para_tmmsm01["ST_NO"].ToString().Trim().Substring(0, 3) == "1A6")
			{
				v_code_wt = 7.95;
			}
			else if (para_tmmsm01["ST_NO"].ToString().Trim().Substring(0, 3) == "1A9")
			{
				v_code_wt = 7.95;
			}
			else if (para_tmmsm01["ST_NO"].ToString().Trim().Substring(0, 2) == "1D")
			{
				v_code_wt = 7.8;
			}
			else if (para_tmmsm01["ST_NO"].ToString().Trim().Substring(0, 1) == "1")
			{
				v_code_wt = 7.9;
			}
			else if (para_tmmsm01["ST_NO"].ToString().Trim().Substring(0, 1) == "2")
			{
				v_code_wt = 7.85;
			}
			else if (para_tmmsm01["ST_NO"].ToString().Trim().Substring(0, 1) == "3")
			{
				v_code_wt = 7.85;
			}*/

			doFlag = f_mmsm_get_density(para_tmmsm01["ST_NO"].ToString(), v_code_wt,conn);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}


			para_tmmsm01["PRODUTE_CAL_WT"] = ((para_tmmsm01["MAT_THICK"].ToDecimal() / 1000) * (para_tmmsm01["MAT_WIDTH"].ToDecimal() / 1000) * (para_tmmsm01["MAT_LEN"].ToDecimal() / 1000) * v_code_wt).Round(3);

		}


		//改炉号和改材料号的情况
		if (bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString() != bcls_rec->Tables["YSJL"].Rows[0]["MAT_NO"].ToString()
			|| bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString() != bcls_rec->Tables["YSJL"].Rows[0]["HEAT_NO"].ToString())
		{
			Log::Trace("", __FUNCTION__, "para_tmmsm01.MAT_NO= [{0}]   [{1}]", para_tmmsm01["MAT_NO"].ToString(), bcls_rec->Tables["YSJL"].Rows[0]["MAT_NO"].ToString());

			//获取制造命令号
			sqlstr = " SELECT PONO FROM TPSSM11 WHERE HEAT_NO = '" + para_tmmsm01["HEAT_NO"].ToString().Trim() + "' AND PONO_STATUS >= '83' AND STEEL_RETURN_CODE =' '";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				para_tmmsm01["PONO"] = cmd_inq.GetString(1);
				v_upd_flag = 1;// 1表示将命令坯状态更新为未使用  0表示不更新
			}
			else
			{
				sqlstr = " SELECT PONO FROM TPSSM41 WHERE HEAT_NO = '" + para_tmmsm01["HEAT_NO"].ToString().Trim() + "'  AND STEEL_RETURN_CODE =' '";
				cmd_inq_pono.SetCommandText(sqlstr);
				cmd_inq_pono.ExecuteReader();
				if (cmd_inq_pono.Read())
				{
					para_tmmsm01["PONO"] = cmd_inq_pono.GetString(1);
					v_upd_flag = 0;// 1表示将命令坯状态更新为未使用  0表示不更新
				}
				cmd_inq_pono.Close();

			}
			cmd_inq.Close();


			//材料号校验
			if (para_tmmsm01["MAT_NO"].ToString().GetLength() != 10)
			{
				strcpy(s.msg, "材料号必须是10位数据，请重新确认！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}



			//获取原始记录数据
			tmmsm01_ysjl.MergeFrom(bcls_rec->Tables["YSJL"].Rows[0]);

			tmmsm01["MAT_NO"] = tmmsm01_ysjl["MAT_NO"];
			tmmsm01.Query("MAT_NO");
			tmmsm01.TrimOrBlank();
			tmmsm01_t8ps02.CopyFrom(tmmsm01);
			//只有发送过的才发撤销
			if (tmmsm01_t8ps02["HR_SEND_FLAG"].ToString().Trim() == "1")
			{

				tmmsm01_t8ps02.MergeTo(inblock.Tables[0], false);
				doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
				if (doFlag < 0) {
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			//tmmsm01_t8ps02.CopyFrom(tmmsm01);
			//如果已经收货了的，要先收货撤销才能改炉号
			//判断收货成功和收货重量
			if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() == "S" || tmmsm01["RECEIVE_WEIGHT"].ToDecimal() != 0)
			{
				sprintf(s.msg, "材料[%s]已收货，不能修改炉号和材料号，请先进行收货撤销!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "N")
			{
				sprintf(s.msg, "材料[%s]当前状态不为未收货，不能修改钢种炉号和材料号，请先进行收货撤销!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//修改炉号，材料号，必须是余材才可以   mfj  20240416   不卡条件，直接脱合同， 脱命令坯
			/*if (tmmsm01["LSLAB_NO"].ToString().Trim() != "")
			{
				sprintf(s.msg, "材料[%s]请先脱虚拟板坯号，再进行修改操作!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/


			if (para_tmmsm01["SLAB_NO"].ToString().Trim().GetLength() > 24)
			{
				sprintf(s.msg, "材料[%s]板坯号不为正确板坯号，请重新确认数据后，再进行修改操作!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (tmmsm01["MAT_ID"].ToString().Trim() == "")
			{
				sprintf(s.msg, "材料[%s]不在当前档!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (para_tmmsm01["MAT_NO"].ToString().GetLength() == 10)
			{
				para_tmmsm01["MAT_WT"] = para_tmmsm01["MAT_WT"].ToDecimal().Round(3);
				para_tmmsm01["MAT_ACT_WT"] = para_tmmsm01["MAT_ACT_WT"].ToDecimal().Round(3);
				para_tmmsm01["MAT_THEORY_WT"] = para_tmmsm01["MAT_THEORY_WT"].ToDecimal().Round(3);
				para_tmmsm01["REAL_TIME_WT"] = para_tmmsm01["REAL_TIME_WT"].ToDecimal().Round(3);
				para_tmmsm01["QUALIFIED_WT"] = para_tmmsm01["QUALIFIED_WT"];
				para_tmmsm01["RECEIVE_WEIGHT"] = para_tmmsm01["RECEIVE_WEIGHT"];
			}
			else
			{
				para_tmmsm01["MAT_WT"] = tmmsm01["MAT_WT"].ToDecimal().Round(3);
				para_tmmsm01["MAT_ACT_WT"] = tmmsm01["MAT_ACT_WT"].ToDecimal().Round(3);
				para_tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"].ToDecimal().Round(3);
				para_tmmsm01["REAL_TIME_WT"] = tmmsm01["REAL_TIME_WT"].ToDecimal().Round(3);
				para_tmmsm01["QUALIFIED_WT"] = tmmsm01["QUALIFIED_WT"];
				para_tmmsm01["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"];

			}


			//当传入的钢种与当前钢种不一致时 根据钢种查询钢牌号，将钢牌号更新掉  工艺卡牌号  跟sg_sign不同，sg_sign从tpssm03表获取
			if (tmmsm01["ST_NO"].ToString() != para_tmmsm01["ST_NO"].ToString())
			{
				sqlstr = "SELECT  SG_GRADE_1,C_DIV  FROM TQMTS0X  WHERE ST_NO = '" + para_tmmsm01["ST_NO"].ToString().Trim() + "'";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					para_tmmsm01["SG_GRADE_1"] = cmd_inq.GetString(1);
					para_tmmsm01["C_DIV"] = cmd_inq.GetString(2);
				}
				//para_tmmsm01["ORDER_NO"] = " ";//脱合同，不抛合同跟踪  mfj 潘  20240410
				cmd_inq.Close();
			}

			//将不为12的碳锈区分，改为12
			if (para_tmmsm01["C_DIV"].ToString().Trim() != "")
			{
				if (para_tmmsm01["C_DIV"].ToString().Trim() == "4")
				{
					para_tmmsm01["C_DIV"] = "1";
				}

				if (para_tmmsm01["C_DIV"].ToString().Trim() == "3" || para_tmmsm01["C_DIV"].ToString().Trim() == "5")
				{
					para_tmmsm01["C_DIV"] = "2";
				}
			}



			//目前暂定碳钢改碳钢，不锈钢改不锈钢   碳锈区分中非12的都改成12(产出时)  mfj  20240416
			if (tmmsm01["C_DIV"].ToString() != para_tmmsm01["C_DIV"].ToString())
			{
				sprintf(s.msg, "材料[%s]只允许碳钢改碳钢，不锈钢改不锈钢，请重新确认钢种!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//当熔炼号不同，计划号相同时，认为现场忘改计划号，此处获取一下  mfj  20240415
			//不论前台数据是啥，这里重新获取计划号
			sqlstr = "SELECT SM_PLAN_NOL2  FROM TPSSM11 WHERE HEAT_NO = '" + para_tmmsm01["HEAT_NO"].ToString() + "' ";

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				para_tmmsm01["SM_PLAN_NO"] = cmd_inq.GetString(1);
			}
			else
			{
				sqlstr = "SELECT SM_PLAN_NOL2  FROM TPSSM41 WHERE HEAT_NO = '" + para_tmmsm01["HEAT_NO"].ToString() + "' ";

				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					para_tmmsm01["SM_PLAN_NO"] = cmd_inq.GetString(1);
				}
				else
				{
					sprintf(s.msg, "未获取到修改炉次的计划信息，请等板坯切割信息后修改!");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				
			}
			cmd_inq.Close();
			

			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM04";
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
			bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
			bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "mmsm01a1f4_upd";
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm01_ysjl["MAT_NO"];

			tmmsm01_ysjl.MergeTo(in_23m.Tables[1]);
			in_23m.Tables[1].Rows[0]["MAT_STATUS"] = "D";

			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


			//带有虚拟板坯的坯子将虚拟板坯回退成未开始状态
			for (int i = 1; i < 13; i++)
			{
				if (para_tmmsm01["PONO_SLAB_" + CConvert::ToString(i)].ToString().Trim() != "")
				{
					if (v_upd_flag == 1)
					{
						tpssm03.Reset();
						tpssm03["SLAB_NO"] = tmmsm01["PONO_SLAB_" + CConvert::ToString(i)].ToString().Trim();
						tpssm03["SLAB_PROD_FLAG"] = "0";
						tpssm03.Update("SLAB_PROD_FLAG", "SLAB_NO");
					}
					para_tmmsm01["PONO_SLAB_" + CConvert::ToString(i)] = " ";
				}
			}
			para_tmmsm01["PONO_SLAB"] = " ";
			para_tmmsm01["JUDGE_ST_NO"] = para_tmmsm01["ST_NO"];
			para_tmmsm01["LSLAB_NO"] = " ";
			para_tmmsm01["ORDER_NO"] = " ";
			para_tmmsm01["FACTORY_DIV"] = "LG1";
			para_tmmsm01["RCV_MAT_FLAG"] = "N";
			para_tmmsm01["REAL_TIME_WT"] = para_tmmsm01["MAT_WT"].ToDecimal().Round(3);//实时重量   未收货的修改 只修改实时重量
			//将前台弹窗数据赋给96结构体
			tmmsm96.CopyFrom(para_tmmsm01);
			para_tmmsm01.MergeTo(in_23m.Tables[1]);
			//只有第一次的修改才给初始值赋值
			if (tmmsm96["INITIAL_HEAT_NO"].ToString().Trim() == "")
			{
				tmmsm96["INITIAL_SG_SIGN"] = tmmsm01_ysjl["SG_SIGN"];//初始牌号(钢级)
				tmmsm96["INITIAL_SG_STD"] = tmmsm01_ysjl["SG_STD"];//初始标准
				tmmsm96["INITIAL_ORDER_NO"] = tmmsm01_ysjl["ORDER_NO"];//初始合同号
				tmmsm96["INITIAL_MSC"] = tmmsm01_ysjl["MSC"];//初始冶金规范码
				tmmsm96["INITIAL_PSC"] = tmmsm01_ysjl["PSC"];//初始产品规范码
				tmmsm96["INITIAL_APN"] = tmmsm01_ysjl["APN"];//初始产品最终用途码
				tmmsm96["INITIAL_HEAT_NO"] = tmmsm01_ysjl["HEAT_NO"];//初始熔炼号
				tmmsm96["INITIAL_BATCH"] = tmmsm01_ysjl["BATCH"];//初始批次号
				tmmsm96["INITIAL_ST_NO"] = tmmsm01_ysjl["ST_NO"];//初始出钢记号
				tmmsm96["INITIAL_SLAB_NO"] = tmmsm01_ysjl["SLAB_NO"];//初始板坯号
			}
			tmmsm96["MAT_NO_OLD"] = tmmsm01_ysjl["MAT_NO"];//初始板坯号

			bcls_rec->Tables["MM0099"].Columns.Clear();//先将列清空
			bcls_rec->Tables["MM0099"].Clear();//将数据清空
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);//再将列添加全
			bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
			bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);


			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM2N";
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
			bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
			bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "mmsm01a1f4_upd";
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = "盘库F7修改钢种，炉号，材料号等操作";
			//

			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}



		}
		else
		{

			Log::Trace("", __FUNCTION__, "v_button_check1111= [{0}]", (const char*)v_button_check);

			/* 材料是否在当前档 */
			//tmmsm01["MAT_NO"] = para_tmmsm01["MAT_NO"];
			//tmmsm01.Query("MAT_NO");
			
			//Log::Trace("", __FUNCTION__, "para_tmmsm01.MAT_WT		= [{0}]", para_tmmsm01["MAT_WT"].ToDecimal());
			if (tmmsm01["MAT_ID"].ToString().Trim() == "")
			{
				sprintf(s.msg, "材料[%s]不在当前档!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/*bcls_rec->Tables["YSJL"].Rows[0]["MAT_THICK"] != para_tmmsm01["MAT_THICK"].ToDecimal()
				|| bcls_rec->Tables["YSJL"].Rows[0]["MAT_LEN"] != para_tmmsm01["MAT_LEN"].ToDecimal()
				||*/
			if (v_button_check == "F4_BUTTON")
			{
				if (tmmsm01["MAT_ACT_WT"].ToDecimal() != tmmsm01["RECEIVE_WEIGHT"].ToDecimal()
					&&(bcls_rec->Tables["YSJL"].Rows[0]["MAT_ACT_WT"] != para_tmmsm01["MAT_WT"].ToDecimal()))
				{
					strcpy(s.msg, "材料收货重量与系统重量不一致，不可进行修改操作");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				
			}

			if (v_button_check == "F7_BUTTON")
			{
				if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "N")
				{
					sprintf(s.msg, "材料[%s]当前状态不为未收货，不能修改钢种炉号和材料号，请先进行收货撤销!", (const char*)tmmsm01["MAT_NO"].ToString());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			tmmsm01_t8ps02.CopyFrom(tmmsm01);
			//只有发送过的才发撤销
			if (tmmsm01_t8ps02["HR_SEND_FLAG"].ToString().Trim() == "1")
			{

				tmmsm01_t8ps02.MergeTo(inblock.Tables[0], false);
				doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
				if (doFlag < 0) {
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}

			/* 校验逻辑数据 */
			/*if(tmmsm01["HOLD_FLAG"].ToString().Trim() ==	"0")
			{
			sprintf(s.msg,"没有封锁[%s]的材料[%s]不能修改!",(const char*)tmmsm01["HOLD_FLAG"].ToString() ,(const char*)tmmsm01["MAT_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
			}*/
			if (tmmsm01["MAT_STATUS"].ToString().Trim() == "24"
				|| tmmsm01["MAT_STATUS"].ToString().Trim() == "34"
				|| tmmsm01["MAT_STATUS"].ToString().Trim() == "36"
				|| tmmsm01["MAT_STATUS"].ToString().Trim() == "38")
			{
				sprintf(s.msg, "材料[%s]已经编入计划,不允许修改!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmmsm01["REPAIR_FLAG"].ToString().Trim() == "1")
			{
				sprintf(s.msg, "材料[%s]目前正在返修中,不允许修改!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmmsm01["TRANSFER_FLAG"].ToString().Trim() == "1")
			{
				sprintf(s.msg, "材料[%s]已经编入转库计划中,不允许修改!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 修改材料主档 */
			//para_tmmsm01["MAT_ACT_THICK"] = para_tmmsm01["MAT_THICK"];//材料实际厚度
			//para_tmmsm01["MAT_ACT_WIDTH"] = para_tmmsm01["MAT_WIDTH"];//材料实际宽度
			//para_tmmsm01["MAT_ACT_LEN"]	= para_tmmsm01["MAT_LEN"];	//材料实际长度
			//para_tmmsm01["MAT_THEORY_WT"] = (para_tmmsm01["MAT_THICK"].ToDecimal() * para_tmmsm01["MAT_WIDTH"].ToDecimal() * para_tmmsm01["MAT_LEN"].ToDecimal() * para_tmmsm01.SHEET_NUM * 7.85)/1000000000;
			//para_tmmsm01["MAT_THEORY_WT"] = (double)((int)(para_tmmsm01["MAT_THEORY_WT"].ToDecimal().ToDouble() * 1000))/1000; //保留3位小数点
			//Log::Trace(1, 1, "222222222  tmmsm01["MAT_THEORY_WT"] = [%f]", tmmsm01["MAT_THEORY_WT"].ToDecimal().ToDouble());
			
			para_tmmsm01["MAT_THEORY_WT"] = para_tmmsm01["MAT_WT"].ToDecimal().Round(3);

			//未收货的时候修改重量只改实时重量，收货后改重量只改实际和收货重量和合格产量
			if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() == "N")
			{
				para_tmmsm01["REAL_TIME_WT"] = para_tmmsm01["MAT_WT"].ToDecimal().Round(3);//实时重量
			}
			else
			{
				para_tmmsm01["MAT_ACT_WT"] = para_tmmsm01["MAT_WT"].ToDecimal().Round(3);
				if (tmmsm01["RECEIVE_WEIGHT"].ToDecimal() == tmmsm01["MAT_ACT_WT"].ToDecimal())//倒灌回来的数据，修改不改变收货重量和合格重量
				{
					para_tmmsm01["QUALIFIED_WT"] = para_tmmsm01["MAT_ACT_WT"].ToDecimal().Round(3);//合格产量
					para_tmmsm01["RECEIVE_WEIGHT"] = para_tmmsm01["MAT_ACT_WT"].ToDecimal().Round(3);//收货重量一起改掉   mfj  20240307   用作101,102的替代
				}
			}
			
	
			

			// 设置默认值
			para_tmmsm01["MAT_ACT_THICK"] = para_tmmsm01["MAT_THICK"];
			para_tmmsm01["MAT_ACT_WIDTH"] = para_tmmsm01["MAT_WIDTH"];
			para_tmmsm01["MAT_ACT_LEN"] = para_tmmsm01["MAT_LEN"];
			para_tmmsm01["SLAB_HEAD_WIDTH"] = para_tmmsm01["MAT_WIDTH"];//头宽尾宽保持一致   mfj   太钢定制  20231227
			para_tmmsm01["SLAB_TAIL_WIDTH"] = para_tmmsm01["MAT_WIDTH"];
			//para_tmmsm01.MAT_GROSS_WT	= para_tmmsm01["MAT_ACT_WT"].ToDecimal() + para_tmmsm01.PACK_MAT_WT;	/* 材料毛重 */ 
			//Log::Trace("", __FUNCTION__, "para_tmmsm01.MAT_GROSS_WT		= [{0}]",para_tmmsm01.MAT_GROSS_WT);

			//当传入的钢种与当前钢种不一致时 根据钢种查询钢牌号，将钢牌号更新掉
			if (tmmsm01["ST_NO"].ToString() != para_tmmsm01["ST_NO"].ToString())
			{
				if (para_tmmsm01["PONO_SLAB"].ToString().Trim() != "")
				{
					sqlstr = "SELECT  SG_SIGN  FROM TPSSM03  WHERE SLAB_NO = '" + para_tmmsm01["PONO_SLAB"].ToString().Trim() + "'";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						para_tmmsm01["SG_SIGN"] = cmd_inq.GetString(1);
					}
				}
				else
				{
					sqlstr = "SELECT  SG_SIGN  FROM TPSSM03  WHERE PONO = '" + para_tmmsm01["PONO"].ToString().Trim() + "'";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						para_tmmsm01["SG_SIGN"] = cmd_inq.GetString(1);
					}
				}

				//改钢种时 脱订单 脱虚拟板坯
				//带有虚拟板坯的坯子将虚拟板坯回退成未开始状态
				//根据PONO查询计划表，若有数据，则没有炉次关闭，则可以更新命令坯状态
				sqlstr = " SELECT PONO FROM TPSSM11 WHERE PONO = '" + para_tmmsm01["PONO"].ToString().Trim() + "'";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					v_upd_flag = 1;// 1表示将命令坯状态更新为未使用  0表示不更新
				}
				else
				{
					v_upd_flag = 0;// 1表示将命令坯状态更新为未使用  0表示不更新
				}
				cmd_inq.Close();

				Log::Trace("", __FUNCTION__, "v_upd_flag= [{0}]", v_upd_flag);
				para_tmmsm01["LSLAB_NO"] = " ";
				para_tmmsm01["ORDER_NO"] = " ";
				para_tmmsm01["PONO_SLAB"] = " ";
				for (int i = 1; i < 13; i++)
				{
					if (para_tmmsm01["PONO_SLAB_" + CConvert::ToString(i)].ToString().Trim() != "")
					{
						if (v_upd_flag == 1)
						{
							tpssm03.Reset();
							tpssm03["SLAB_NO"] = tmmsm01["PONO_SLAB_" + CConvert::ToString(i)].ToString().Trim();
							tpssm03["SLAB_PROD_FLAG"] = "0";
							tpssm03.Update("SLAB_PROD_FLAG", "SLAB_NO");
							Log::Trace("", __FUNCTION__, "SLAB_NO= [{0}]", tpssm03["SLAB_NO"].ToString());
						}
						para_tmmsm01["PONO_SLAB_" + CConvert::ToString(i)] = " ";
					}
				}

			}

			cmd_inq.Close();

			if (tmmsm01["ST_NO"].ToString() != para_tmmsm01["ST_NO"].ToString())
			{
				sqlstr = "SELECT  SG_GRADE_1,C_DIV  FROM TQMTS0X  WHERE ST_NO = '" + para_tmmsm01["ST_NO"].ToString().Trim() + "'";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					para_tmmsm01["SG_GRADE_1"] = cmd_inq.GetString(1);
					para_tmmsm01["C_DIV"] = cmd_inq.GetString(2);
				}
				cmd_inq.Close();
			}

			//将不为12的碳锈区分，改为12
			if (para_tmmsm01["C_DIV"].ToString().Trim() != "")
			{
				if (para_tmmsm01["C_DIV"].ToString().Trim() == "4")
				{
					para_tmmsm01["C_DIV"] = "1";
				}

				if (para_tmmsm01["C_DIV"].ToString().Trim() == "3" || para_tmmsm01["C_DIV"].ToString().Trim() == "5")
				{
					para_tmmsm01["C_DIV"] = "2";
				}
			}

			Log::Trace("", __FUNCTION__, "v_button_check2222= [{0}]", (const char*)v_button_check);

			//目前暂定碳钢改碳钢，不锈钢改不锈钢   碳锈区分中非12的都改成12(产出时)  mfj  20240416
			if (tmmsm01["C_DIV"].ToString() != para_tmmsm01["C_DIV"].ToString())
			{
				sprintf(s.msg, "材料[%s]只允许碳钢改碳钢，不锈钢改不锈钢，请重新确认钢种!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			para_tmmsm01["JUDGE_ST_NO"] = para_tmmsm01["ST_NO"];
			para_tmmsm01.MergeTo(in_23m.Tables[1]);
			/* 调用物料总函数f_mmsm99 */
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM03";
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
			bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
			bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "mmsm01a1f4_upd";
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = para_tmmsm01["MAT_NO"];
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_THICK"] = para_tmmsm01["MAT_THICK"];
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_WIDTH"] = para_tmmsm01["MAT_WIDTH"];
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_LEN"] = para_tmmsm01["MAT_LEN"];
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_THICK"] = para_tmmsm01["MAT_ACT_THICK"];
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_WIDTH"] = para_tmmsm01["MAT_ACT_WIDTH"];
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_LEN"] = para_tmmsm01["MAT_ACT_LEN"];

			//针对分切的子坯，可以修改规格，不可以修改重量
			if (para_tmmsm01["MAT_NO"].ToString().GetLength() == 12 && para_tmmsm01["IN_MAT_NO"].ToString().Trim() !="")
			{
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_WT"] = tmmsm01["MAT_WT"].ToDecimal().Round(3);
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_WT"] = tmmsm01["MAT_ACT_WT"].ToDecimal().Round(3);
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"].ToDecimal().Round(3);
				bcls_rec->Tables["MM0099"].Rows[0]["REAL_TIME_WT"] = tmmsm01["REAL_TIME_WT"].ToDecimal().Round(3);
				bcls_rec->Tables["MM0099"].Rows[0]["QUALIFIED_WT"] = tmmsm01["QUALIFIED_WT"];
				bcls_rec->Tables["MM0099"].Rows[0]["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"];
			}
			else
			{
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_WT"] = para_tmmsm01["MAT_WT"].ToDecimal().Round(3);
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_WT"] = para_tmmsm01["MAT_ACT_WT"].ToDecimal().Round(3);
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_THEORY_WT"] = para_tmmsm01["MAT_THEORY_WT"].ToDecimal().Round(3);
				bcls_rec->Tables["MM0099"].Rows[0]["REAL_TIME_WT"] = para_tmmsm01["REAL_TIME_WT"].ToDecimal().Round(3);
				bcls_rec->Tables["MM0099"].Rows[0]["QUALIFIED_WT"] = para_tmmsm01["QUALIFIED_WT"];
				bcls_rec->Tables["MM0099"].Rows[0]["RECEIVE_WEIGHT"] = para_tmmsm01["RECEIVE_WEIGHT"];
			}
			bcls_rec->Tables["MM0099"].Rows[0]["PRODUTE_CAL_WT"] = para_tmmsm01["PRODUTE_CAL_WT"].ToDecimal().Round(3);
			//bcls_rec->Tables["MM0099"].Rows[0]["MAT_GROSS_WT"]		= para_tmmsm01.MAT_GROSS_WT; 
			bcls_rec->Tables["MM0099"].Rows[0]["MEASURE_WT_FLAG"] = para_tmmsm01["MEASURE_WT_FLAG"];
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_NUM"] = para_tmmsm01["MAT_NUM"];
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_TUBE"] = para_tmmsm01["MAT_NUM"];
			bcls_rec->Tables["MM0099"].Rows[0]["SURF_QUALITY"] = para_tmmsm01["SURF_QUALITY"];
			bcls_rec->Tables["MM0099"].Rows[0]["BATCH"] = para_tmmsm01["BATCH"];
			bcls_rec->Tables["MM0099"].Rows[0]["HEAT_NO"] = para_tmmsm01["HEAT_NO"];
			bcls_rec->Tables["MM0099"].Rows[0]["REMARK"] = para_tmmsm01["REMARK"];
			bcls_rec->Tables["MM0099"].Rows[0]["CASTING_PURPOSE"] = para_tmmsm01["CASTING_PURPOSE"];//铸坯用途
			bcls_rec->Tables["MM0099"].Rows[0]["PRINT_NO"] = para_tmmsm01["PRINT_NO"];//喷印号
			bcls_rec->Tables["MM0099"].Rows[0]["PONO"] = para_tmmsm01["PONO"];//--LZ  20240315
			bcls_rec->Tables["MM0099"].Rows[0]["SM_PLAN_NO"] = para_tmmsm01["SM_PLAN_NO"];//--LZ  20240315
			//bcls_rec->Tables["MM0099"].Rows[0]["POLISH_GRADE"] = para_tmmsm01["POLISH_GRADE"];
			bcls_rec->Tables["MM0099"].Rows[0]["SLAB_NO"] = para_tmmsm01["SLAB_NO"];
			bcls_rec->Tables["MM0099"].Rows[0]["ST_NO"] = para_tmmsm01["ST_NO"];
			bcls_rec->Tables["MM0099"].Rows[0]["SG_SIGN"] = para_tmmsm01["SG_SIGN"];
			bcls_rec->Tables["MM0099"].Rows[0]["SPECIFICATIONS"] = para_tmmsm01["SPECIFICATIONS"];
			bcls_rec->Tables["MM0099"].Rows[0]["ADJUST_WIDTH_MARK"] = para_tmmsm01["ADJUST_WIDTH_MARK"];
			bcls_rec->Tables["MM0099"].Rows[0]["ORDER_NO"] = para_tmmsm01["ORDER_NO"];
			bcls_rec->Tables["MM0099"].Rows[0]["ZL_REASON_DESC"] = para_tmmsm01["ZL_REASON_DESC"];
			bcls_rec->Tables["MM0099"].Rows[0]["JUDGE_RESULT_1"] = para_tmmsm01["JUDGE_RESULT_1"];
			bcls_rec->Tables["MM0099"].Rows[0]["TRIMTEXT"] = para_tmmsm01["TRIMTEXT"];
			bcls_rec->Tables["MM0099"].Rows[0]["STEEL_GROUP"] = para_tmmsm01["STEEL_GROUP"];
			bcls_rec->Tables["MM0099"].Rows[0]["SG_GRADE_1"] = para_tmmsm01["SG_GRADE_1"];
			bcls_rec->Tables["MM0099"].Rows[0]["JUDGE_ST_NO"] = para_tmmsm01["JUDGE_ST_NO"];
			bcls_rec->Tables["MM0099"].Rows[0]["JUDGE_ST_NO"] = para_tmmsm01["CASTING_PRE_JUDGMENT"];
			bcls_rec->Tables["MM0099"].Rows[0]["STOCK_L2"] = para_tmmsm01["STOCK_L2"];
			bcls_rec->Tables["MM0099"].Rows[0]["LSLAB_NO"] = para_tmmsm01["LSLAB_NO"];
			bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB"] = para_tmmsm01["PONO_SLAB"];
			bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_1"] = para_tmmsm01["PONO_SLAB_1"];
			bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_2"] = para_tmmsm01["PONO_SLAB_2"];
			bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_3"] = para_tmmsm01["PONO_SLAB_3"];
			bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_4"] = para_tmmsm01["PONO_SLAB_4"];
			bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_5"] = para_tmmsm01["PONO_SLAB_5"];
			bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_6"] = para_tmmsm01["PONO_SLAB_6"];
			bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_7"] = para_tmmsm01["PONO_SLAB_7"];
			bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_8"] = para_tmmsm01["PONO_SLAB_8"];
			bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_9"] = para_tmmsm01["PONO_SLAB_9"];
			bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_10"] = para_tmmsm01["PONO_SLAB_10"];
			bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_11"] = para_tmmsm01["PONO_SLAB_11"];
			bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_12"] = para_tmmsm01["PONO_SLAB_12"];
			if (v_button_check == "F4_BUTTON")
			{
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = "盘库F4修改板坯规格尺寸";
			}
			else if (v_button_check == "F7_BUTTON")
			{
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] =  "盘库F7修改板坯钢种，规格，尺寸";
			}
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//
			//#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
			//			/* 抛合同跟踪 */
			//			if (tmmsm01["ORDER_NO"].ToString().Trim() != "") //材料修改
			//			{
			//				tpmof03["EVENT_ID"] = "54";//  材料修改
			//				tpmof03["ORDER_NO"] = tmmsm01["ORDER_NO"].ToString();
			//				tpmof03["SYSTEM_ID"] = "MM";
			//				tpmof03["FUNC_ID"] = "mmsm0001f4_upd";
			//				tpmof03["WHOLE_BACKLOG"] = tmmsm01["WHOLE_BACKLOG"].ToString();
			//				tpmof03["WHOLE_BACKLOG_SEQ"] = tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"];
			//				tpmof03["WHOLE_BACKLOG_CODE"] = tmmsm01["NEXT_WHOLE_BACKLOG_CODE"].ToString();
			//				tpmof03["MAT_NO"] = tmmsm01["MAT_NO"].ToString();
			//				tpmof03["MAT_STATUS"] = tmmsm01["MAT_STATUS"].ToString();
			//				tpmof03["WT"] = para_tmmsm01["MAT_WT"];
			//				tpmof03["PREV_MAT_NO"] = tmmsm01["MAT_NO"].ToString();
			//				tpmof03["PREV_MAT_STATUS"] = tmmsm01["MAT_STATUS"];
			//				tpmof03["PREV_WT"] = tmmsm01["MAT_WT"];
			//				if (tmmsm01["PLAN_NO"].ToString().Trim() != "")
			//				{
			//					tpmof03["IF_PLAN"] = "1";	//计划中标志 1=计划中 		
			//				}
			//				else
			//				{
			//					tpmof03["IF_PLAN"] = "0";	//计划中标志  		
			//				}
			//				tpmof03.MergeTo(bcls_rec->Tables["PMOF99"], false);
			//				doFlag = f_pmof99_v3(bcls_rec, bcls_ret, conn);
			//				if (doFlag < 0)
			//				{
			//					throw CApplicationException(doFlag, s.msg, log.Location);
			//				}
			//			}
			//#endif

		}

		//F4 收货后   F7是未收货 
		//这里只有F4的时候发送电文
		if (v_button_check == "F4_BUTTON" || v_button_check == "F7_BUTTON")
		{
			CString old_if_hr = Db::QueryCString(" select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE =(select GUIDE_DEST from tmmsm01 where MAT_NO='" + para_tmmsm01["MAT_NO"].ToString() + "') ");
			if (old_if_hr.Find("1") >= 0 && (para_tmmsm01["MAT_THICK"] != tmmsm01["MAT_THICK"]
				|| para_tmmsm01["MAT_WIDTH"] != tmmsm01["MAT_WIDTH"]
				|| para_tmmsm01["MAT_LEN"] != tmmsm01["MAT_LEN"]
				|| para_tmmsm01["MAT_WT"] != tmmsm01["MAT_WT"]
				|| para_tmmsm01["ST_NO"] != tmmsm01["ST_NO"]
				|| para_tmmsm01["PRINT_NO"] != tmmsm01["PRINT_NO"]
				|| para_tmmsm01["SLAB_NO"] != tmmsm01["SLAB_NO"]))
			{
				
				

				//防止改了材料号，前面的查询为空导致没有数据，故这里再查一遍。保证有数据 mfj  20240521
				tmmsm01["MAT_NO"] = para_tmmsm01["MAT_NO"];
				tmmsm01.Query("MAT_NO");
				tmmsm01.TrimOrBlank();

				
				

				inblock.Tables[0].Rows.Clear();
				para_tmmsm01.MergeTo(inblock.Tables[0], false);
				doFlag = f_wmsm_t8p301_snd(&inblock, bcls_ret, conn);
				if (doFlag < 0) {
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			if (v_button_check == "F7_BUTTON")
			{
				EIClass bcls_E2T8M1;
				bcls_E2T8M1.Tables[0].set_TableName("E2T8M1");
				bcls_E2T8M1.Tables["E2T8M1"].Columns.Add(DT_STRING, "MAT_NO");
				bcls_E2T8M1.Tables["E2T8M1"].Rows.Add();
				bcls_E2T8M1.Tables["E2T8M1"].Rows[0]["MAT_NO"] = para_tmmsm01["MAT_NO"];
				
				doFlag = f_mmsm_e2t8m1_snd(&bcls_E2T8M1, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}

#pragma region 调用函数，发送智慧质量电文
		if (in_23m.Tables[1].Rows.get_Count() > 0)
		{
			doFlag = f_t8z_23m_snd(&in_23m, bcls_ret, conn);
		}
#pragma endregion
		//发送宝武聊天 表面质量封锁 暂不发，还没维护好小代码
		//if (tmmsm01["SURF_QUALITY"].ToString().TrimOrBlank() != "0" || tmmsm01["SURF_QUALITY"].ToString().TrimOrBlank() != " ")
		//{

		//	//查询对应的中文名字  表面质量
		//	CString code_desc_surt = "";
		//	CString strsql = " select CODE_DESC_1_CONTENT from TWMSMZD02  WHERE CODE_CLASS = 'MMBMZL' AND CODE = '" + tmmsm01["SURF_QUALITY"].ToString() + "'";
		//	cmd_inq1.SetCommandText(strsql);
		//	cmd_inq1.ExecuteReader();
		//	if (cmd_inq1.Read())
		//	{
		//		code_desc_surt = cmd_inq1.GetString(1);
		//	}
		//	cmd_inq1.Close();

		//	Log::Trace("", __FUNCTION__, "code_desc_surt[{0}]  ", code_desc_surt);

		//	bcls_rec_s.Tables[0].Rows.Add();
		//	bcls_rec_s.Tables[0].Rows[0]["SURF_QUALITY"] = code_desc_surt;
		//	bcls_rec_s.Tables[0].Rows[0]["CODE"] = "6";
		//	bcls_rec_s.Tables[0].Rows[0]["REMARK"] = tmmsm01["MAT_NO"];
		//	doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
		//	if (doFlag < 0)
		//	{
		//		strcpy(s.msg, "调用函数报错!");
		//		throw CApplicationException(-1, s.msg, log.Location);
		//	}
		//}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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

	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}

