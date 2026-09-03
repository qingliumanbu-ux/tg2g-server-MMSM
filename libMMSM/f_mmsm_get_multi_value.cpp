/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: ZY
日期: 2014-10-29 11:28:43
功能: 从字符串中根据指定分隔符拆分数据，并以数组方式返回。
修改历史：
日期:________；修改人：________; 需求提出人________
变更内容:
**************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include
#include "stdafx.h" 
#define zero 0.00001

/*<remark>=========================================================
/// <summary>
///  从字符串中根据指定分隔符拆分数据，并以数组方式返回。
/// <para>
///    功能叙述段落
//    前台传入的信息[101, 102, 103]
//    返回输出信息是个字符数组==> v_out_str99[]

////v_spilit_len=分隔符长度,需根据‘分隔符长度’，灵活进行数据拆分。 UPDATE ON 2018-5-31 10:48:54
/// </para>
/// </summary>
/// <param name="bcls_rec">传入块  </param>
/// <param name="bcls_ret">返回块  </param>
/// <returns>返回：是否成功</returns>
===========================================================</remark>*/

// 
//v_in_str= 传入的字符串。
//v_spilit_flag=指定的分隔符。

BM2_FUNCTION_EXPORT
int f_mmsm_get_multi_value(CString v_in_str, CString v_spilit_flag, CString *v_out_str99, int *v_out_str_cnt, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm_get_multi_value";                //定义函数英文名称  
	CString FunctionCname = "从字符串中根据指定分隔符拆分数据，并以数组方式返回";              //定义函数中文名称

	//Log::Trace("", __FUNCTION__, "IN== new address =TEST ");
 


	//程序用变量
	int   doFlag = 0;
	int   i = 0;
	int   find_id = 0;
	int   find_start = 0;
	int   str_len = 0;
	CString sqlstr = "";

	int   row_num = 0; //数组的个数。
	
	//设定返回的数组变量。
	//CString v_out_str99[200] = {""}; //字符型数组.暂定200行。


	//CString v_in_str = ""; //传入的字符信息。
	//CString v_spilit_flag = ",";//分隔符号。
	CString v_out_str_tmp = ""; //返回的字符信息。 

	CString str = "";
	CString str2 = "";
	CString str3 = "";




	try
	{

    //举例如下：
		//入口信息是[101, 102, 103],需要进行拆分处理。
		//返回输出信息是个字符型数组v_out_str99[],数组中一共3行，内容是101 102 103 。

		//Log::Trace("", __FUNCTION__, "IN:v_in_str = [{0}]  ", v_in_str);
	 
		if(v_in_str.Trim() == "")
		{
			sprintf(s.msg, "入口信息为空[%s]没有内容需要拆分处理，直接离开。", (const char*)v_in_str);
			//Log::Trace("", __FUNCTION__, "IN:s.msg = [{0}]  ", s.msg);
			*v_out_str_cnt = -1; //数组中的内容没有。
			doFlag = 0;
			s.flag = doFlag;
			return doFlag;
			//throw CApplicationException(-1, s.msg, log.Location);
			
		}


		//根据陈添提醒，防止黑客，需要把一些特殊符号过滤掉。
		//，'  /*  */  -- 
		//===========on 2015-7-1 10:50:23
		//v_in_str = v_in_str.Replace(",", "");
		v_in_str = v_in_str.Replace("'", "");
		//Log::Trace("", __FUNCTION__, "'==v_in_str[{0}] ,test。 ", v_in_str);
		v_in_str = v_in_str.Replace("/*", "");
		//Log::Trace("", __FUNCTION__, "/*==v_in_str[{0}] ,test。 ", v_in_str);
		v_in_str = v_in_str.Replace("*/", "");
		//Log::Trace("", __FUNCTION__, "*/==v_in_str[{0}] ,test。 ", v_in_str);
		v_in_str = v_in_str.Replace("--", "");
		//Log::Trace("", __FUNCTION__, "--==v_in_str[{0}] ,test。 ", v_in_str);
		//v_in_str = v_in_str.Replace("or", "");
		//Log::Trace("", __FUNCTION__, "or ==v_in_str[{0}] ,test。 ", v_in_str);
		//Log::Trace("", __FUNCTION__, "--防黑处理结果 ==>v_in_str[{0}]  ", v_in_str);


		/*
		//需要将[特殊符号]去掉后，判断一下是否有值。
		//制表符= \t  回车符 = \r 换行符= \n 
		*/
		CString v_tmp = "";
		CString v_tmp2 = "";
		CString v_tmp3 = "";

		v_tmp = v_in_str.Replace("\t","");
		v_tmp2 = v_tmp.Replace("\r","");
		v_tmp3 = v_tmp2.Replace("\n","");

		if(v_tmp3.Trim() == "")
		{//若信息]中，除去特殊符号后，没有内容了，则不作为查询条件。 

			//Log::Trace("",__FUNCTION__,"v_tmp3[{0}] ,当前字段不不作为查询条件。 ",v_tmp3);
			sprintf(s.msg, "入口信息[%s]除了特殊字符，没有内容需要拆分处理。", (const char*)v_in_str);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//需要将[制表符]去掉， 
		//制表符= \t  回车符 = \r 换行符= \n


		//将制表符 替换成 空格
		str = v_in_str.Replace("\t"," ");
		
		int v_spilit_len = 0; //分隔符的长度。
		v_spilit_len = v_spilit_flag.GetLength();
		//Log::Trace("", __FUNCTION__, "--分隔符=[{0}]长度[{1}]  ", v_spilit_flag, v_spilit_len);
		find_start = 0; //查找的开始位置。
		find_id    = 0;
		find_id = str.Find(v_spilit_flag,find_start); //查找分隔符是[,]
		if(find_id < 0)
		{//从头找也没找到指定的分隔符,则说明只有一个值，直接处理后返回。

			//sprintf(s.msg,"v_in_str[%s]中不包括分隔符[%s]，直接离开本函数",(const char*)str,(const char*)v_spilit_flag);
			//throw CApplicationException(-1, s.msg, log.Location); 


			// 101--->返回的是  101，写入返回数组遍历中。
			//v_out_str = "'" + str.Trim() + "'"; //v_out_str ='101' 

			////过滤回车符，过滤换行符
			str2 = str2.Replace("\r", "");
			str2 = str2.Replace("\n", "");

			row_num = 0;
			v_out_str99[row_num] = str.Trim();

		}
		else
		{//存在分隔符。

			row_num = 0; //数组中的个数。
			find_start = 0;
			find_id    = 0;
			for(i = 1;i <= str.GetLength();i++)
			{
				find_id = str.Find(v_spilit_flag,find_start); //查找分隔符是[,]
				
				if(find_id < 0)
				{
					str2 = str.Substring(find_start,str.GetLength()-find_start).Trim();
					
					//str3 = str3 + ",'" + str2 + "' ";//添加一个 ['101']
					v_out_str99[row_num] = str2.Trim();
					break;
				}
				str2 = str.Substring(find_start,find_id-find_start).Trim();
				//find_start = find_id + 1;
				find_start = find_id + v_spilit_len; //开始查找位置= 当前找到位置+分隔符长度
				 
				//str3 = str3 + ",'" + str2 + "'"; //添加一个 [,'101'] 

				////过滤回车符，过滤换行符
				str2 = str2.Replace("\r","");
				str2 = str2.Replace("\n","");

				v_out_str99[row_num] = str2.Trim();
				row_num++;

			} 

			

			////去掉第一个逗号。
			//str_len = str3.GetLength();
			//str3 = str3.Substring(1,str_len-1).Trim(); //str3 ='101','102','103'
			//v_out_str = str3;  //v_out_str ='101','102','103'


		} 

		

		*v_out_str_cnt = row_num;  
		//显示返回的数组信息。
		for (i = 0; i <= row_num; i++)
		{
			v_out_str_tmp = v_out_str99[i];
			//Log::Trace("", __FUNCTION__, "i [{0}] v_out_str_tmp[{1}] ", i, v_out_str_tmp);
			 
		}


		////过滤回车符，过滤换行符
		//v_out_str2 = v_out_str.Replace("\r","");
		//v_out_str3 = v_out_str2.Replace("\n","");

		//Log::Trace("",__FUNCTION__,"IN:v_in_str = [{0}] v_out_str3 =[{1}]",v_in_str,v_out_str3); 

		/*设置系统返回参数*/
		//处理成功。 
		//strcpy(s.msg, "恭喜，处理成功。");	



	}

	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CString str = "DB error:[" + sqlstr + "]\r\n" + ex.GetMsg();
		sprintf(s.msg, "%s,sqlCode[%d]", (const char*)str, ex.GetCode());
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

	s.flag = doFlag;
	return(doFlag);
}