/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2013
Author:      顾云峰
Version:     1.0
Date:        2013-08-21
Description: 根据板坯主档中的命令板坯号个数生成目的板坯表
**************************************************/
/*<remark>============================================================================
/// <summary>
/// 根据板坯主档中的命令板坯号个数生成目的板坯表
/// <para>
/// </para>
/// </summary>
/// <param name="">                                                           </param>
/// <returns>                                                               </returns>
============================================================================</remark>*/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient; 

#include "EI_TUXClass.h"

/* ***** 程序表结构引用 ***** */
//#include "tmmsm01.h" 
//#include "tmmsm03.h"
//#include "tmmsm04.h" 
//#include "tmmsm96.h"

//外部函数声明
int f_mmsm_status(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);
int f_mmsm0005_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);
int f_mmsm0004_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);
int f_mmsm0008_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);

int f_mmsm0007_subBklgIns(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);

//发送电文

int f_mmsm0001_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	EIClass bcls_rec_t;
	EIClass bcls_ret_t;

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int atFlag = 0;
	int i;
	int j;

	int com_num=0;                  //命令板坯数量n
	int remain_num=0;               //余材数量（无合同）Q
	int blkNum;
	int fetchRowCount;
	CString com_pono_no[8];
	CString datetime;
	double L_x;						//原坯长度与各个定尺坯（命令板坯）之和的差(x)
	double L_w;						//原坯重量与各个定尺坯（命令板坯）之和的差(w)
	float sum_pre_clean_slab_max_len=0;
	float sum_pre_clean_slab_min_len=0;
	float sum_pre_clean_slab_len=0;
	double sum_pre_clean_slab_max_wt=0;
	double sum_pre_clean_slab_min_wt=0;
	double sum_pre_clean_slab_wt=0;
	float Remain_len;				//余长坯长度
	double Remain_wt;				//余长坯重量
	float Len_max;					//由各个命令板坯反算原坯总长（最大）
	float Len_min;					//由各个命令板坯反算原坯总长（最小）
	float Len;						//由各个命令板坯反算原坯总长
	double Wt_max;					//由各个命令板坯反算原坯总重（最大）
	double Wt_min;					//由各个命令板坯反算原坯总重（最小）
	double Wt;						//由各个命令板坯反算原坯总重
	int i_col_index = 0;
	CString cust_mat_no = "";
	int i_updown_flag = 0;          //余材是头或尾标记
	int i_matWtFlag = 0;            //重量标记，当实物板坯的理论重量 > 命令板坯清理前重量之和时，设置为1 （2011-02-23吴珊修改）
    CString orderDiv = " ";
	CString userid = " ";

	CString v_orderNo = " ";
	int i_count;
	int i_cut_seq = 0;
	CString com_pono = " ";
	CString aim_mat_no = " ";  //已修改
	double prod_density = 7.7;		//材料密度
	CString order_remain_div = " ";		//合同材/预备材区分(TPMOUHP30)
	float pre_clean_slab_max_len;		//清理前板坯最大长度(TPMOUHP30)
	float pre_clean_slab_min_len;		//清理前板坯最小长度(TPMOUHP30)
	float pre_clean_slab_len;			//清理前板坯长度(TPMOUHP30)
	double pre_clean_slab_max_wt;		//清理前板坯最大重量(TPMOUHP30)
	double pre_clean_slab_min_wt;		//清理前板坯最小重量(TPMOUHP30)
	double pre_clean_slab_wt;			//清理前板坯重量(TPMOUHP30)
	int slab_cut_gap = 10;			//长板坯充当短板坯之间切缝宽度(TSIPMHP20)
	int ord_com_num=0;              //有合同的命令板坯数量m
	CString v_ponoSlab = ""; //已修改
	double v_slabMinWt;
	CString v_order_remain_div="";

	//使用的表结构变量
	CModel tmmsm01("TMMSM01");
	CModel tmmsm03("TMMSM03");
	CModel tmmsm04("TMMSM04");
	
	/*CTMMSM01 tmmsm01(conn);
	CTMMSM03 tmmsm03(conn);
    CTMMSM04 tmmsm04(conn);*/

	CString  sqlstr("");
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq2(conn);
	
	try
	{
		if(bcls_rec->Tables.Contains("MMSM0001") == false)
		{
			strcpy(s.msg,_RES("GCRSS0000011")/*系统出现异常，数据块有误，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location); 
		}

		if(bcls_rec->Tables.Contains("MMSM0005") == false)
		{
			bcls_rec->Tables.Add("MMSM0005");
			bcls_rec->Tables["MMSM0005"].Columns.Add(DT_STRING,"mat_no");
			bcls_rec->Tables["MMSM0005"].Columns.Add(DT_STRING,"pono_slab");
			bcls_rec->Tables["MMSM0005"].Columns.Add(DT_STRING,"fix_slab_num");
			bcls_rec->Tables["MMSM0005"].Columns.Add(DT_STRING,"cut_seq");
			bcls_rec->Tables["MMSM0005"].Columns.Add(DT_STRING,"mat_wt_flag");
			bcls_rec->Tables["MMSM0005"].Rows.Add();
		}

		if(bcls_rec->Tables.Contains("MMSM0004") == false)
		{
			bcls_rec->Tables.Add("MMSM0004");
			bcls_rec->Tables["MMSM0004"].Columns.Add(DT_STRING,"mat_no");
			bcls_rec->Tables["MMSM0004"].Columns.Add(DT_STRING,"infur_slab_wt");
			bcls_rec->Tables["MMSM0004"].Columns.Add(DT_STRING,"aim_mat_no");
			bcls_rec->Tables["MMSM0004"].Columns.Add(DT_STRING,"cust_mat_no");
			bcls_rec->Tables["MMSM0004"].Columns.Add(DT_STRING,"infur_slab_len");
			bcls_rec->Tables["MMSM0004"].Columns.Add(DT_STRING,"infur_slab_thick");
			bcls_rec->Tables["MMSM0004"].Columns.Add(DT_STRING,"infur_slab_wid");
			bcls_rec->Tables["MMSM0004"].Columns.Add(DT_STRING,"order_remain_div");
			bcls_rec->Tables["MMSM0004"].Columns.Add(DT_STRING,"order_no");
			bcls_rec->Tables["MMSM0004"].Columns.Add(DT_STRING, "PONO_SLAB");
			bcls_rec->Tables["MMSM0004"].Rows.Add();
		}

		if(bcls_rec->Tables.Contains("MMSM0008") == false)
		{
			bcls_rec->Tables.Add("MMSM0008");
			bcls_rec->Tables["MMSM0008"].Columns.Add(DT_STRING,"mat_no");
			bcls_rec->Tables["MMSM0008"].Columns.Add(DT_STRING,"cut_num");
			bcls_rec->Tables["MMSM0008"].Columns.Add(DT_STRING,"cut_seq");
			bcls_rec->Tables["MMSM0008"].Rows.Add();
		}

		if(bcls_rec->Tables.Contains("MM0099") == false)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"mat_no");
			bcls_rec->Tables["MM0099"].Rows.Add();
		}

		//获取传入参数
		tmmsm01.MergeFrom(bcls_rec->Tables["MMSM0001"].Rows[0]);

		Log::Trace("",__FUNCTION__,"mat_no = [{0}]",tmmsm01["MAT_NO"].ToString());

		/*校验传入参数*/
		if (tmmsm01["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,_RES("GCRSS0000035")/*材料号不能为空。*/);
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		
		tmmsm01["REC_REVISE_TIME"] = s.datetime;
		tmmsm01["REC_REVISOR"] = s.userid;

		tmmsm01.Query("MAT_NO");

		//删除tmmsm03 tmmsm04
		tmmsm03["MAT_NO"] = tmmsm01["MAT_NO"];
		tmmsm03.Delete("MAT_NO");

		tmmsm04["MAT_NO"] = tmmsm01["MAT_NO"];
		tmmsm04.Delete("MAT_NO");
		
		CDataTable dt;
		tmmsm01.MergeTo(dt,false);

		Log::Trace("",__FUNCTION__,"pono_slab_count = [{0}]",dt.Columns.get_Count());

		for(i_col_index = 0;i_col_index < dt.Columns.get_Count(); i_col_index ++)
		{
			

			if(dt.Columns[i_col_index].get_ColumnName().Find("PONO_SLAB_") < 0)
			{
				continue;
			}

			Log::Trace("",__FUNCTION__,"i_col_index = [{0}][{1}][{2}]",i_col_index,dt.Columns[i_col_index].get_ColumnName(),dt[0][i_col_index].ToString());

			if(dt[0][i_col_index].ToString() == " ")
			{
				continue;
			}

			com_num = com_num + 1;
			com_pono = dt[0][i_col_index].ToString();
			
			order_remain_div = "0";
			cmd_inq.SetCommandText(" SELECT TRIM(DECODE(ORDER_NO,' ',0,1)) ORDER_REMAIN_DIV,SLAB_MIN_LEN,SLAB_MAX_LEN,SLAB_LEN,SLAB_MIN_WT,SLAB_MAX_WT,SLAB_WT FROM TPSSM03 WHERE SLAB_NO = @com_pono");
			cmd_inq.Parameters.Set("com_pono",com_pono);
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				order_remain_div = cmd_inq.GetString(1);
				pre_clean_slab_min_len = cmd_inq.GetFloat(2);
				pre_clean_slab_max_len = cmd_inq.GetFloat(3);
				pre_clean_slab_len = cmd_inq.GetFloat(4);
				pre_clean_slab_min_wt = cmd_inq.GetFloat(5);
				pre_clean_slab_max_wt = cmd_inq.GetFloat(6);
				pre_clean_slab_wt = cmd_inq.GetFloat(7);
			}
			cmd_inq.Close();

			if(order_remain_div == "1") //合同材命令板坯
			{
				ord_com_num = ord_com_num + 1;

				com_pono_no[ord_com_num - 1] = com_pono;
				
				Log::Trace("",__FUNCTION__,"第[{0}]个合同材命令板坯com_pono_no[{1}]",ord_com_num,com_pono_no[ord_com_num-1]);

				if(ord_com_num == 1)
				{
//					cmd_inq.SetCommandText(" SELECT PROD_DENSITY FROM TPMOUHP31 WHERE PONO_SLAB = @com_pono ");
//					cmd_inq.Parameters.Set("com_pono",com_pono);
//					prod_density = cmd_inq.ExecuteScalar().ToDouble();
//					cmd_inq.Close();
//					if(prod_density == 0.0) prod_density = 7.85;
						

					/*tmmsm01["PONO_SLAB"] = com_pono;
					tmmsm01.Update("PONO_SLAB","MAT_NO");*/

				}
				
				sum_pre_clean_slab_min_len = sum_pre_clean_slab_min_len + pre_clean_slab_min_len;
				sum_pre_clean_slab_max_len = sum_pre_clean_slab_max_len + pre_clean_slab_max_len;
				sum_pre_clean_slab_len     = sum_pre_clean_slab_len     + pre_clean_slab_len;
				sum_pre_clean_slab_min_wt  = sum_pre_clean_slab_min_wt  + pre_clean_slab_min_wt;
				sum_pre_clean_slab_max_wt  = sum_pre_clean_slab_max_wt  + pre_clean_slab_max_wt;
				sum_pre_clean_slab_wt      = sum_pre_clean_slab_wt      + pre_clean_slab_wt;
			}
			else remain_num = remain_num + 1;
		}

		Log::Trace("",__FUNCTION__,"ord_com_num = [{0}]",ord_com_num);
		Log::Trace("",__FUNCTION__,"remain_num  = [{0}]",remain_num);
		Log::Trace("",__FUNCTION__,"com_num     = [{0}]",com_num);

		Len_max = sum_pre_clean_slab_max_len + slab_cut_gap*(com_num - 1);	
		Len_min = sum_pre_clean_slab_min_len + slab_cut_gap*(com_num - 1);
		Len     = sum_pre_clean_slab_len     + slab_cut_gap*(com_num - 1);
		Wt_max  = sum_pre_clean_slab_max_wt  + slab_cut_gap*(com_num - 1)*prod_density * tmmsm01["MAT_ACT_THICK"].ToDouble() * tmmsm01["MAT_ACT_WIDTH"].ToDouble() / 1000000000;	
		//EDLog(1,1,"命令板坯最大重量之和（考虑切缝）,Wt_max***[%f]***",Wt_max);			
		Wt_min  = sum_pre_clean_slab_min_wt  + slab_cut_gap*(com_num - 1)*prod_density * tmmsm01["MAT_ACT_THICK"].ToDouble() * tmmsm01["MAT_ACT_WIDTH"].ToDouble() / 1000000000;
		//EDLog(1,1,"命令板坯最小重量之和（考虑切缝）,Wt_min***[%f]***",Wt_min);
		Wt      = sum_pre_clean_slab_wt      + slab_cut_gap*(com_num - 1)*prod_density * tmmsm01["MAT_ACT_THICK"].ToDouble() * tmmsm01["MAT_ACT_WIDTH"].ToDouble() / 1000000000;
		//EDLog(1,1,"命令板坯目标重量之和（考虑切缝）,Wt***[%f]***",Wt);
		tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_ACT_LEN"].ToDouble() * tmmsm01["MAT_ACT_THICK"].ToDouble() * tmmsm01["MAT_ACT_WIDTH"].ToDouble() * prod_density/1000000000;				//板坯理论重量
		//EDLog(1,1,"实物材料理论重量,tmmsm01.mat_theory_wt***[%f]***",tmmsm01.mat_theory_wt);
		Remain_len = tmmsm01["MAT_ACT_LEN"].ToDouble() - sum_pre_clean_slab_len - slab_cut_gap*ord_com_num;//余长坯长度
		//EDLog(1,1,"余长坯长度,Remain_len***[%ld]***",Remain_len);
		Remain_wt  = prod_density * Remain_len * tmmsm01["MAT_ACT_THICK"].ToDouble() * tmmsm01["MAT_ACT_WIDTH"].ToDouble() / 1000000000;	 //余长坯重量
		//EDLog(1,1,"余长坯重量,Remain_wt***[%f]***",Remain_wt);

		if (tmmsm01["MAT_THEORY_WT"].ToDouble() - Wt >= 0.001)
		{
			i_matWtFlag = 1;
		}
		
		//开始处理
		if(ord_com_num == 0 && remain_num >= 1)
		{
			//所有命令板坯都无合同
			bcls_rec->Tables["MMSM0004"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_wt"] = tmmsm01["MAT_THEORY_WT"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["aim_mat_no"] = tmmsm01["MAT_NO"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["cust_mat_no"] = tmmsm01["MAT_NO"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_len"] = tmmsm01["MAT_ACT_LEN"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_thick"] = tmmsm01["MAT_ACT_THICK"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_wid"] = tmmsm01["MAT_ACT_WIDTH"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["order_remain_div"] = "0";
			bcls_rec->Tables["MMSM0004"].Rows[0]["PONO_SLAB"] = com_pono;

			doFlag = f_mmsm0004_proc(bcls_rec, bcls_ret,conn);
			if(doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		if(ord_com_num >= 1 && remain_num >= 1)
		{
			//有合同。有余材
			/*tmmsm01["FIX_SLAB_NUM"] = ord_com_num + 1;
			tmmsm01.Update("FIX_SLAB_NUM","MAT_NO");*/

			if(tmmsm01["SLAB_PLACE_CODE"].ToString() == "B") //若板坯头尾标记为Ｂ，则将长坯上的所有余材都置到板坯头部
			{
				//生成余长坯号
				bcls_rec->Tables["MMSM0008"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
				bcls_rec->Tables["MMSM0008"].Rows[0]["cut_num"] = ord_com_num + 1;
				bcls_rec->Tables["MMSM0008"].Rows[0]["cut_seq"] = "1";

				doFlag = f_mmsm0008_proc(bcls_rec, bcls_ret,conn);
				if(doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				aim_mat_no = bcls_rec->Tables["MMSM0008"].Rows[0]["mat_no"].ToString();

				bcls_rec->Tables["MMSM0004"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
				bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_wt"] = Remain_wt;
				bcls_rec->Tables["MMSM0004"].Rows[0]["aim_mat_no"] = aim_mat_no;
				bcls_rec->Tables["MMSM0004"].Rows[0]["cust_mat_no"] = " ";
				bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_len"] = Remain_len;
				bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_thick"] = tmmsm01["MAT_ACT_THICK"];
				bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_wid"] = tmmsm01["MAT_ACT_WIDTH"];
				bcls_rec->Tables["MMSM0004"].Rows[0]["order_remain_div"] = "0";
				bcls_rec->Tables["MMSM0004"].Rows[0]["PONO_SLAB"] = com_pono;

				doFlag = f_mmsm0004_proc(bcls_rec, bcls_ret,conn);
				if(doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				
				//生成合同材的目的档
				for(i=1;i<=ord_com_num;i++)
				{
					bcls_rec->Tables["MMSM0005"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
					bcls_rec->Tables["MMSM0005"].Rows[0]["pono_slab"] = com_pono_no[i-1];
					bcls_rec->Tables["MMSM0005"].Rows[0]["fix_slab_num"] = ord_com_num+1;
					bcls_rec->Tables["MMSM0005"].Rows[0]["cut_seq"] = i+1;
					bcls_rec->Tables["MMSM0005"].Rows[0]["mat_wt_flag"] = i_matWtFlag;

					doFlag = f_mmsm0005_proc(bcls_rec, bcls_ret,conn);
					if(doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
			}
			else //若板坯头尾标记为Ｔ，则将长坯上的所有余材都置到板坯尾部
			{
				//生成合同材的目的档
				for(i=1;i<=ord_com_num;i++)
				{
					bcls_rec->Tables["MMSM0005"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
					bcls_rec->Tables["MMSM0005"].Rows[0]["pono_slab"] = com_pono_no[i-1];
					bcls_rec->Tables["MMSM0005"].Rows[0]["fix_slab_num"] = ord_com_num+1;
					bcls_rec->Tables["MMSM0005"].Rows[0]["cut_seq"] = i;
					bcls_rec->Tables["MMSM0005"].Rows[0]["mat_wt_flag"] = i_matWtFlag;

					doFlag = f_mmsm0005_proc(bcls_rec, bcls_ret,conn);
					if(doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}

				//生成余长坯号
				bcls_rec->Tables["MMSM0008"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
				bcls_rec->Tables["MMSM0008"].Rows[0]["cut_num"] = ord_com_num + 1;
				bcls_rec->Tables["MMSM0008"].Rows[0]["cut_seq"] = ord_com_num + 1;

				doFlag = f_mmsm0008_proc(bcls_rec, bcls_ret,conn);
				if(doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				aim_mat_no = bcls_rec->Tables["MMSM0008"].Rows[0]["mat_no"].ToString();

				bcls_rec->Tables["MMSM0004"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
				bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_wt"] = Remain_wt;
				bcls_rec->Tables["MMSM0004"].Rows[0]["aim_mat_no"] = aim_mat_no;
				bcls_rec->Tables["MMSM0004"].Rows[0]["cust_mat_no"] = " ";
				bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_len"] = Remain_len;
				bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_thick"] = tmmsm01["MAT_ACT_THICK"];
				bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_wid"] = tmmsm01["MAT_ACT_WIDTH"];
				bcls_rec->Tables["MMSM0004"].Rows[0]["order_remain_div"] = "0";
				bcls_rec->Tables["MMSM0004"].Rows[0]["PONO_SLAB"] = com_pono;

				doFlag = f_mmsm0004_proc(bcls_rec, bcls_ret,conn);
				if(doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
		}

		if(ord_com_num >= 1 && remain_num == 0)
		{
			//都是合同板坯
			if (tmmsm01["MAT_THEORY_WT"].ToDouble() >= Wt_min && tmmsm01["MAT_THEORY_WT"].ToDouble() <= Wt_max)
			{
				//重量符合
			/*	tmmsm01["FIX_SLAB_NUM"] = ord_com_num;
				tmmsm01.Update("FIX_SLAB_NUM","MAT_NO");*/

				if(ord_com_num == 1)
				{
					//生成一对一的目的板坯
					bcls_rec->Tables["MMSM0005"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
					bcls_rec->Tables["MMSM0005"].Rows[0]["pono_slab"] = com_pono_no[0];
					bcls_rec->Tables["MMSM0005"].Rows[0]["fix_slab_num"] = ord_com_num;
					bcls_rec->Tables["MMSM0005"].Rows[0]["cut_seq"] = 1;
					bcls_rec->Tables["MMSM0005"].Rows[0]["mat_wt_flag"] = i_matWtFlag;

					doFlag = f_mmsm0005_proc(bcls_rec, bcls_ret,conn);
					if(doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
				else
				{
					//生成一对多的目的板坯
					for(i=1;i<=ord_com_num;i++)
					{
						bcls_rec->Tables["MMSM0005"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
						bcls_rec->Tables["MMSM0005"].Rows[0]["pono_slab"] = com_pono_no[i-1];
						bcls_rec->Tables["MMSM0005"].Rows[0]["fix_slab_num"] = ord_com_num;
						bcls_rec->Tables["MMSM0005"].Rows[0]["cut_seq"] = i;
						bcls_rec->Tables["MMSM0005"].Rows[0]["mat_wt_flag"] = i_matWtFlag;

						doFlag = f_mmsm0005_proc(bcls_rec, bcls_ret,conn);
						if(doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
				}
			}
			else if (tmmsm01["MAT_THEORY_WT"].ToDouble()> Wt_max)
			{
				//实物重量大于命令板坯最大重量之和
				L_w = tmmsm01["MAT_THEORY_WT"].ToDouble() - Wt;
				L_x = L_w / prod_density / tmmsm01["MAT_ACT_THICK"].ToDouble() / tmmsm01["MAT_ACT_WIDTH"].ToDouble() * 1000000000;

				if(Remain_len >=1500)
				{
					//余长>=1500mm或者1000mm<=余长<1500但宽度>=1500，则产生余长坯
					/*tmmsm01["FIX_SLAB_NUM"] = ord_com_num + 1;
					tmmsm01.Update("FIX_SLAB_NUM","MAT_NO");*/

					if(tmmsm01["SLAB_PLACE_CODE"].ToString() == "B") //若板坯头尾标记为Ｂ，则将长坯上的所有余材都置到板坯头部
					{
						//生成余长坯号
						bcls_rec->Tables["MMSM0008"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
						bcls_rec->Tables["MMSM0008"].Rows[0]["cut_num"] = ord_com_num + 1;
						bcls_rec->Tables["MMSM0008"].Rows[0]["cut_seq"] = "1";

						doFlag = f_mmsm0008_proc(bcls_rec, bcls_ret,conn);
						if(doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
						aim_mat_no = bcls_rec->Tables["MMSM0008"].Rows[0]["mat_no"].ToString();

						bcls_rec->Tables["MMSM0004"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
						bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_wt"] = Remain_wt;
						bcls_rec->Tables["MMSM0004"].Rows[0]["aim_mat_no"] = aim_mat_no;
						bcls_rec->Tables["MMSM0004"].Rows[0]["cust_mat_no"] = " ";
						bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_len"] = Remain_len;
						bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_thick"] = tmmsm01["MAT_ACT_THICK"];
						bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_wid"] = tmmsm01["MAT_ACT_WIDTH"];
						bcls_rec->Tables["MMSM0004"].Rows[0]["order_remain_div"] = "0";
						bcls_rec->Tables["MMSM0004"].Rows[0]["PONO_SLAB"] = com_pono;

						doFlag = f_mmsm0004_proc(bcls_rec, bcls_ret,conn);
						if(doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
						
						//生成合同材的目的档
						for(i=1;i<=ord_com_num;i++)
						{
							bcls_rec->Tables["MMSM0005"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
							bcls_rec->Tables["MMSM0005"].Rows[0]["pono_slab"] = com_pono_no[i-1];
							bcls_rec->Tables["MMSM0005"].Rows[0]["fix_slab_num"] = ord_com_num+1;
							bcls_rec->Tables["MMSM0005"].Rows[0]["cut_seq"] = i+1;
							bcls_rec->Tables["MMSM0005"].Rows[0]["mat_wt_flag"] = i_matWtFlag;

							doFlag = f_mmsm0005_proc(bcls_rec, bcls_ret,conn);
							if(doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
					}
					else //若板坯头尾标记为Ｔ，则将长坯上的所有余材都置到板坯尾部
					{
						//生成合同材的目的档
						for(i=1;i<=ord_com_num;i++)
						{
							bcls_rec->Tables["MMSM0005"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
							bcls_rec->Tables["MMSM0005"].Rows[0]["pono_slab"] = com_pono_no[i-1];
							bcls_rec->Tables["MMSM0005"].Rows[0]["fix_slab_num"] = ord_com_num+1;
							bcls_rec->Tables["MMSM0005"].Rows[0]["cut_seq"] = i;
							bcls_rec->Tables["MMSM0005"].Rows[0]["mat_wt_flag"] = i_matWtFlag;

							doFlag = f_mmsm0005_proc(bcls_rec, bcls_ret,conn);
							if(doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}

						//生成余长坯号
						bcls_rec->Tables["MMSM0008"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
						bcls_rec->Tables["MMSM0008"].Rows[0]["cut_num"] = ord_com_num + 1;
						bcls_rec->Tables["MMSM0008"].Rows[0]["cut_seq"] = ord_com_num + 1;

						doFlag = f_mmsm0008_proc(bcls_rec, bcls_ret,conn);
						if(doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
						aim_mat_no = bcls_rec->Tables["MMSM0008"].Rows[0]["mat_no"].ToString();

						bcls_rec->Tables["MMSM0004"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
						bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_wt"] = Remain_wt;
						bcls_rec->Tables["MMSM0004"].Rows[0]["aim_mat_no"] = aim_mat_no;
						bcls_rec->Tables["MMSM0004"].Rows[0]["cust_mat_no"] = " ";
						bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_len"] = Remain_len;
						bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_thick"] = tmmsm01["MAT_ACT_THICK"];
						bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_wid"] = tmmsm01["MAT_ACT_WIDTH"];
						bcls_rec->Tables["MMSM0004"].Rows[0]["order_remain_div"] = "0";

						doFlag = f_mmsm0004_proc(bcls_rec, bcls_ret,conn);
						if(doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
				}
				else //L_x<1500
				{
					/*tmmsm01["FIX_SLAB_NUM"] = ord_com_num;
					tmmsm01.Update("FIX_SLAB_NUM","MAT_NO");*/

					if(ord_com_num == 1)
					{
						//生成一对一的目的板坯
						bcls_rec->Tables["MMSM0005"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
						bcls_rec->Tables["MMSM0005"].Rows[0]["pono_slab"] = com_pono_no[0];
						bcls_rec->Tables["MMSM0005"].Rows[0]["fix_slab_num"] = ord_com_num;
						bcls_rec->Tables["MMSM0005"].Rows[0]["cut_seq"] = 1;
						bcls_rec->Tables["MMSM0005"].Rows[0]["mat_wt_flag"] = i_matWtFlag;

						doFlag = f_mmsm0005_proc(bcls_rec, bcls_ret,conn);
						if(doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
					else
					{
						//生成一对多的目的板坯
						for(i=1;i<=ord_com_num;i++)
						{
							bcls_rec->Tables["MMSM0005"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
							bcls_rec->Tables["MMSM0005"].Rows[0]["pono_slab"] = com_pono_no[i-1];
							bcls_rec->Tables["MMSM0005"].Rows[0]["fix_slab_num"] = ord_com_num;
							bcls_rec->Tables["MMSM0005"].Rows[0]["cut_seq"] = i;
							bcls_rec->Tables["MMSM0005"].Rows[0]["mat_wt_flag"] = i_matWtFlag;

							doFlag = f_mmsm0005_proc(bcls_rec, bcls_ret,conn);
							if(doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
					}
				}
			}
			else
			{
				//实物重量小于命令板坯最小重量之和
				/*tmmsm01["FIX_SLAB_NUM"] = ord_com_num;
				tmmsm01.Update("FIX_SLAB_NUM","MAT_NO");*/

				if(ord_com_num == 1)
				{
					//生成一对一的目的板坯
					bcls_rec->Tables["MMSM0005"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
					bcls_rec->Tables["MMSM0005"].Rows[0]["pono_slab"] = com_pono_no[0];
					bcls_rec->Tables["MMSM0005"].Rows[0]["fix_slab_num"] = ord_com_num;
					bcls_rec->Tables["MMSM0005"].Rows[0]["cut_seq"] = 1;
					bcls_rec->Tables["MMSM0005"].Rows[0]["mat_wt_flag"] = i_matWtFlag;

					doFlag = f_mmsm0005_proc(bcls_rec, bcls_ret,conn);
					if(doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
				else
				{
					//生成一对多的目的板坯
					for(i=1;i<=ord_com_num;i++)
					{
						bcls_rec->Tables["MMSM0005"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
						bcls_rec->Tables["MMSM0005"].Rows[0]["pono_slab"] = com_pono_no[i-1];
						bcls_rec->Tables["MMSM0005"].Rows[0]["fix_slab_num"] = ord_com_num;
						bcls_rec->Tables["MMSM0005"].Rows[0]["cut_seq"] = i;
						bcls_rec->Tables["MMSM0005"].Rows[0]["mat_wt_flag"] = i_matWtFlag;

						doFlag = f_mmsm0005_proc(bcls_rec, bcls_ret,conn);
						if(doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
				}
			}		
		}

		if(ord_com_num == 0 && remain_num == 0)
		{
			//无合同无余材
			bcls_rec->Tables["MMSM0004"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_wt"] = tmmsm01["MAT_THEORY_WT"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["aim_mat_no"] = tmmsm01["MAT_NO"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["cust_mat_no"] = tmmsm01["MAT_NO"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_len"] = tmmsm01["MAT_ACT_LEN"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_thick"] = tmmsm01["MAT_ACT_THICK"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["infur_slab_wid"] = tmmsm01["MAT_ACT_WIDTH"];
			bcls_rec->Tables["MMSM0004"].Rows[0]["order_remain_div"] = "0";

			doFlag = f_mmsm0004_proc(bcls_rec, bcls_ret,conn);
			if(doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		//发送目的板坯生成电文
		//TODO:

		//设置板坯带温切割标记
		/*cmd_inq.SetCommandText(" SELECT DISTINCT SLAB_WARM_CUT_CODE FROM TPMOUHP30 WHERE SLAB_WARM_CUT_CODE > ' ' AND PONO_SLAB IN (@p1,@p2,@p3,@p4,@p5,@p6,@p7,@p8) fetch first 1 rows ONLY ");
		cmd_inq.Parameters.Set("p1",tmmsm01.PONO_SLAB_1);
		cmd_inq.Parameters.Set("p2",tmmsm01.PONO_SLAB_2);
		cmd_inq.Parameters.Set("p3",tmmsm01.PONO_SLAB_3);
		cmd_inq.Parameters.Set("p4",tmmsm01.PONO_SLAB_4);
		cmd_inq.Parameters.Set("p5",tmmsm01.PONO_SLAB_5);
		cmd_inq.Parameters.Set("p6",tmmsm01.PONO_SLAB_6);
		cmd_inq.Parameters.Set("p7",tmmsm01.PONO_SLAB_7);
		cmd_inq.Parameters.Set("p8",tmmsm01.PONO_SLAB_8);
		cmd_inq.ExecuteReader();
		cmd_inq.Close();*/

		//生成主档工序
		//cmd_inq.SetCommandText("SELECT COUNT(0) FROM TMMSM03 WHERE MAT_NO = @tmmsm01.mat_no AND ORDER_REMAIN_DIV = '1' ");
		//cmd_inq.Parameters.Set("tmmsm01.mat_no",tmmsm01["MAT_NO"]);
		//i_count = cmd_inq.ExecuteScalar().ToInt32();
		//cmd_inq.Close();

		//if(i_count > 0)
		//{
		//	//EDIT NVL SQL BY ZHAOLIYUAN 20200411
		//	sqlstr = "SELECT SUB_BACKLOG_CODE,SUB_BACKLOG_SEQ FROM TMMSM04 "
		//		"WHERE AIM_MAT_NO IN (SELECT AIM_MAT_NO FROM TMMSM03 WHERE MAT_NO = @tmmsm01.mat_no AND ORDER_REMAIN_DIV = '1') "
		//		"  AND SUB_BACKLOG_SEQ = ( SELECT NVL(MIN(SUB_BACKLOG_SEQ),0) FROM TMMSM04 WHERE MAT_NO = @tmmsm01.mat_no AND BACKLOG_PASS_TIME = ' ') "
		//			 "  ORDER BY AIM_MAT_NO";
		//	cmd_inq.SetCommandText(sqlstr);
		//	cmd_inq.Parameters.Set("tmmsm01.mat_no",tmmsm01["MAT_NO"]);
		//	cmd_inq.ExecuteReader();
		//	if(cmd_inq.Read())
		//	{
		//		tmmsm01["NEXT_SUB_BACKLOG_CODE"] = cmd_inq.GetString(1);
		//		tmmsm01["NEXT_SUB_BACKLOG_SEQ"] = cmd_inq.GetInt32(2);
		//	}
		//	cmd_inq.Close();
		//}

		////生成主档合同号
		//cmd_inq.SetCommandText("SELECT ORDER_NO FROM TMMSM03 WHERE MAT_NO = @tmmsm01.mat_no AND ORDER_REMAIN_DIV = '1'  ORDER BY AIM_MAT_NO ");
		//cmd_inq.Parameters.Set("tmmsm01.mat_no",tmmsm01["MAT_NO"]);
		//cmd_inq.ExecuteReader();
		//if(cmd_inq.Read()) tmmsm01["ORDER_NO"] = cmd_inq.GetString(1);
		//cmd_inq.Close();

		//tmmsm01.Update("NEXT_SUB_BACKLOG_CODE,NEXT_SUB_BACKLOG_SEQ,ORDER_NO","MAT_NO");
		//
		//bcls_rec->Tables["MM0099"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
		////doFlag = f_mmsm_status(bcls_rec,bcls_ret,conn);
		//if(doFlag < 0)
		//{
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		

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