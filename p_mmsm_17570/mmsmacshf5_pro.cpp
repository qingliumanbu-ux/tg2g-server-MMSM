/************************/
/*** 2023-11-13 ********/
/****   mfj **************/
/**** 收货确认取消 ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件

int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsmacsh_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wmsmsm_stock_in(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//调用仓库接口，进行板坯入库 
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
// service入口
BM2F_ENTERACE(mmsmacshf5_pro)

int f_mmsmacshf5_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;
	CString v_update = "";
	CString s_formname = "";
	CString i_func_id = "";
	CString i_func = "";
	CString v_fields_str = "";
	CString v_resume_seq_no = "";//序号
	int blkNum = 0;	
	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm33("TMMSM33");
	CModel tmmsm33_old("TMMSM33");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm01_query("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm39("TMMSM39");//用来判断收货取消时坯子有没有做过309切废
	CModel tpssm03("TPSSM03");
	CModel tmmsm3e("TMMSM3E");
	CModel twmsmzd02("TWMSMZD02");
	CDbCommand cmd_inq(conn);

	try
	{

		Log::Trace("", __FUNCTION__, "COUTNT[{0}]  ", bcls_rec->Tables[0].Rows.get_Count());

		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}

		blkNum = bcls_rec->Tables.IndexOf("MMSMACSH");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSMACSH");
			bcls_rec->Tables["MMSMACSH"].Columns.Add(tmmsm96);
		}
		bcls_rec->Tables["MMSMACSH"].Columns.Add(DT_STRING, "DEAL_FLAG");

		/**  调用仓库接口  进行入库操作    mfj  2023.12.19  太钢定制 **/
		/*置板坯命令产出标志*/
		blkNum = bcls_rec->Tables.IndexOf("WM_STOCK");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("WM_STOCK");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");        //库业务类型
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_DECIMAL, "STOCK_OPER_ORDER_DIV");   //业务类型内区分
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_NO");				//库号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_NO");			//材料库位号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "ROWNO");					//行号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "COLUMN_NO");				//列号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "LAYERNO");					//层号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_POSITION");	//库位内位置
		}



		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//将结构体清空
			tmmsm01.Reset();
			tmmsm96.Reset();
			tmmsm01_query.Reset();
			

			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm01_query.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm33_old.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (!tmmsm01.QueryCount("MAT_NO"))//TMMSM01表未获取数据
			{
				Log::Trace("", __FUNCTION__, "MAT_NO[{0}]  ", tmmsm01["MAT_NO"].ToString());
				strcpy(s.sysmsg, "在线档中不存在数据，请确认数据是否已归档!");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm01_query.Query("MAT_NO");
			tmmsm33_old.Query("MAT_NO");

			//增加校验，只有余材时，才可以更改成品标记
			/*if (tmmsm01_query["PRODUCT_FLAG"].ToString().Trim() != tmmsm01["PRODUCT_FLAG"].ToString().Trim()
				&& tmmsm01_query["PONO_SLAB"].ToString().Trim() != "")
			{
				sprintf(s.sysmsg, "材料[%s]必须是余材时，才可更改成品标记！！", (const char*)tmmsm01_query["MAT_NO"].ToString());
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}*/

			//将L4层的判断添加在这里
			/*if (!(tmmsm01_query["MAT_STATUS"].ToString() == "20" || tmmsm01_query["MAT_STATUS"].ToString() == "29" || tmmsm01_query["MAT_STATUS"].ToString() == "30" || tmmsm01_query["MAT_STATUS"].ToString() == "39" || (tmmsm01_query["MAT_STATUS"].ToString() == "23" && tmmsm01_query["COMPLEX_DECIDE_CODE"].ToString() == "0")))
			{
				Log::Debug("", __FUNCTION__, "材料不为产出等待状态，不能进行102冲销。" );
				sprintf(s.sysmsg, "材料不为产出等待状态，不能进行102冲销！！");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}*/
			if (tmmsm01_query["MAT_STATUS"].ToString() != "20" && tmmsm01_query["MAT_STATUS"].ToString() != "30" && !(tmmsm01_query["MAT_STATUS"].ToString() == "23" && tmmsm01_query["COMPLEX_DECIDE_CODE"].ToString() == "0") && !(tmmsm01_query["MAT_STATUS"].ToString() == "29" && tmmsm01_query["COMPLEX_DECIDE_CODE"].ToString() == "0") && !(tmmsm01_query["MAT_STATUS"].ToString() == "39" && tmmsm01_query["COMPLEX_DECIDE_CODE"].ToString() == "0") && !(tmmsm01_query["MAT_STATUS"].ToString() == "2A" && tmmsm01_query["COMPLEX_DECIDE_CODE"].ToString() == "0"))
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料不为产出等待状态，不能进行102冲销，请重新查询确认!", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}


			//杨姐确认逻辑  做过309切废的不能取消，取消时重量必须与收货重量一致
			if (tmmsm01_query["MAT_ACT_WT"].ToDecimal() != tmmsm01_query["RECEIVE_WEIGHT"].ToDecimal())
			{
				sprintf(s.sysmsg, "材料当前重量[%s]与收货重量不一致[%s]，不符合收货取消条件！！", (const char*)tmmsm01_query["MAT_ACT_WT"].ToString(), (const char*)tmmsm01_query["RECEIVE_WEIGHT"].ToString());
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm01_query["MEND_FLAG"].ToString().Trim() != "" && tmmsm01_query["MEND_FLAG"].ToString().Trim() != "0")
			{
				sprintf(s.sysmsg, "材料[%s]已经修磨，不符合收货取消条件！！", (const char*)tmmsm01_query["MAT_NO"].ToString());
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm33_old["PONO_SLAB_1"].ToString().Trim() == ""&& tmmsm01_query["ORDER_NO"].ToString().Trim() != "")
			{
				sprintf(s.sysmsg, "材料[%s]产出为余材但现在带了合同，不符合收货取消条件！！", (const char*)tmmsm01_query["MAT_NO"].ToString());
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else if (tmmsm33_old["PONO_SLAB_1"].ToString().Trim() != "" && tmmsm01_query["ORDER_NO"].ToString().Trim() != "")
			{
				if (tmmsm01_query["PONO_SLAB_1"].ToString().Trim() == "")
				{
					sprintf(s.sysmsg, "产出时带命令坯，当前无命令板坯号，请脱合同后再收货取消"); //系统错误信息
					strcpy(s.msg, "产出时带命令坯，当前无命令板坯号，请脱合同后再收货取消"); //用户提示信息，国际化
					throw CApplicationException(-1, s.msg, log.Location);
					
				}
				tpssm03["SLAB_NO"] = tmmsm33_old["PONO_SLAB_1"];
				if (tpssm03.Query("SLAB_NO"))
				{
					if (tpssm03["ORDER_NO"].ToString().Trim() != tmmsm01_query["ORDER_NO"].ToString().Trim() && tpssm03["ORDER_NO"].ToString().Trim() != "")
					{
						sprintf(s.sysmsg, "产出时合同号为[" + tpssm03["ORDER_NO"].ToString() + "]，当前合同号为[" + tmmsm01_query["ORDER_NO"].ToString() + "],两者不相同不能收货取消"); //系统错误信息
						strcpy(s.msg, "产出时合同号为[" + tpssm03["ORDER_NO"].ToString() + "]，当前合同号为[" + tmmsm01_query["ORDER_NO"].ToString() + "],两者不相同不能收货取消"); //用户提示信息，国际化
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			if (tmmsm01["USAGE_DECISION"].ToString() != "3030" && tmmsm01["USAGE_DECISION"].ToString().Trim() != "")
			{
				sprintf(s.sysmsg, "综判必须为未判才可进行收货撤销！"); //系统错误信息
				strcpy(s.msg, "综判必须为未判才可进行收货撤销！"); //用户提示信息，国际化
			}
			twmsmzd02["CODE_CLASS"] = "ADMIN";
			twmsmzd02["CODE"] = s.userid;
			if (tmmsm01["RECV_MAT_TIME"].ToString().SubstringNE(0, 6) < datetime.SubstringNE(0, 6) && twmsmzd02.QueryCount("CODE_CLASS,CODE") <= 0)
			{
				strcpy(s.msg, "所收货取消的板坯不在本月，非管理员不可进行收货取消!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			tmmsm39["MAT_NO"] = tmmsm01_query["MAT_NO"];
			if (tmmsm39.QueryCount("MAT_NO")
				&& (tmmsm01["MAT_NO"].ToString().Substring(8, 2) == "00" || tmmsm01["MAT_NO"].ToString().Substring(8, 2) == "99" || tmmsm01["MAT_NO"].ToString().Substring(8, 2) == "AA" || tmmsm01["MAT_NO"].ToString().Substring(8, 2) == "ZZ")
				&& tmmsm01_query["MAT_ACT_WT"].ToDecimal() != tmmsm01_query["RECEIVE_WEIGHT"].ToDecimal())
			{
				sprintf(s.sysmsg, "材料[%s]已做过309切废，不符合收货取消条件！！", (const char*)tmmsm01_query["MAT_NO"].ToString());
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}



			tmmsm33.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			

			//收货重量不允许为0
			/*if (tmmsm01["RECEIVE_WEIGHT"].ToDecimal() == 0 || tmmsm01["RECEIVE_WEIGHT"].ToDecimal() < 0)
			{
				strcpy(s.sysmsg, "收获重量不允许小于等于0，请重新确认!");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}*/

			//收货取消，时间置空,重量置零   mfj  20240131
		

			// 这里注释掉，等待反馈时处理
			/*tmmsm01["RECV_MAT_TIME"] = " ";
			tmmsm01["RECEIVE_WEIGHT"] = 0;

			tmmsm33["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"];
			
			tmmsm33["RCV_MAT_FLAG"] = "W";  //N 未收货  W等待(等L4的反馈)  S收货成功  
			//因有自动收货，当收货取消时，标记不改，重量改，以重量为判断依据
			tmmsm33["RECV_MAT_TIME"] = tmmsm01["RECV_MAT_TIME"];

			tmmsm33.Update("RECEIVE_WEIGHT,RCV_MAT_FLAG,RECV_MAT_TIME", "MAT_NO");*/


			//RECEIVE_WEIGHT,RCV_MAT_FLAG,RECV_MAT_TIME


			//在反馈里处理  mfj   20240424 
			/*tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
			//tmmsm96["RECEIVE_WEIGHT"] = tmmsm01["MAT_WT"];
			//tmmsm01["REAL_TIME_WT"] = tmmsm01["RECEIVE_WEIGHT"];//实时重量 
			tmmsm96["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"];//收货重量，更新实际重量，实际重量即老系统的系统重量   mfj  20240113
			//tmmsm96["MAT_WT"] = tmmsm01["RECEIVE_WEIGHT"];
			tmmsm96["MAT_ACT_WT"] = tmmsm01["RECEIVE_WEIGHT"];

			Log::Trace("", __FUNCTION__, "RECEIVE_WEIGHT[{0}]  ", tmmsm01["RECEIVE_WEIGHT"].ToString());

			//名义规格与实际规格保持一致
			//tmmsm96["MAT_WIDTH"] = tmmsm01["MAT_WIDTH"];
			//tmmsm96["MAT_LEN"] = tmmsm01["MAT_LEN"];
			//tmmsm96["MAT_THICK"] = tmmsm01["MAT_THICK"];

			////画面修改名义，用名义覆盖实际 保持两套一致
			//tmmsm96["MAT_ACT_WIDTH"] = tmmsm01["MAT_WIDTH"];
			//tmmsm96["MAT_ACT_LEN"] = tmmsm01["MAT_LEN"];
			//tmmsm96["MAT_ACT_THICK"] = tmmsm01["MAT_THICK"];

			tmmsm96["RECV_MAT_TIME"] = tmmsm01["RECV_MAT_TIME"];//收货日期
			//tmmsm96["PRODUCT_FLAG"] = tmmsm01["PRODUCT_FLAG"];//成品标记
			tmmsm96["RCV_MAT_FLAG"] = "W";//N 未收货  W等待(等L4的反馈)  S收货成功
			tmmsm96["RCV_NO_STATUS"] = "S";//添加收货取消状态，取消后自动收货不再进行收货  mfj   20240309
			tmmsm96["EVENT_ID"] = "MM3B";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "mmsmacshf4_pro";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "铸坯收货确认取消,等待反馈";*/





			tmmsm33["MAT_NO"] = tmmsm01["MAT_NO"];
			tmmsm33["RCV_MAT_FLAG"] = "W";
			tmmsm33.Update("RCV_MAT_FLAG", "MAT_NO");

			doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm3e.CopyFrom(tmmsm01);
			tmmsm3e["PROD_TIME"] = datetime;
			tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
			tmmsm3e["REMARK"] = "收货撤销等待反馈";
			tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
			tmmsm3e.TrimOrBlank();
			tmmsm3e.Insert();

			//调用事件  //只修改标记   收货取消  将收获标记置为N
			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "MM3F";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = "mmsmacshf5_pro";
			tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
			tmmsm96["RCV_NO_STATUS"] = "S";

			tmmsm96["RCV_MAT_FLAG"] = "W";
			tmmsm96["EVENT_DESC"] = "收货取消等待反馈";

			bcls_rec->Tables["MM0099"].Rows.Add();
			bcls_rec->Tables["MM0099"].Rows[i].Merge(tmmsm96);


			bcls_rec->Tables["MMSMACSH"].Rows.Add();
			bcls_rec->Tables["MMSMACSH"].Rows[i].Merge(tmmsm96);
			bcls_rec->Tables["MMSMACSH"].Rows[i]["DEAL_FLAG"] = "D";


		}

	
		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}


		if (bcls_rec->Tables["MMSMACSH"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsmacsh_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
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
