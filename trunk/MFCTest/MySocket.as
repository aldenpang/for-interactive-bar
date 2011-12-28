package    
{   
        import flash.net.Socket;   
        import flash.events.*;   
        import flash.display.Sprite;   
        import flash.errors.*;   
        import flash.display.SimpleButton;   
		import flash.display.Stage;
		import flash.utils.ByteArray;
		import Main;
        public class MySocket extends Sprite   
        {   
                private var mysocket:Socket;   
                private var host:String="127.0.0.1";   
                private var port:int=5000;   
				var m:Main;
				private var mStage:Stage;
                var bytes:ByteArray = new ByteArray;
				
                public function MySocket()   
                {    
                        trace("构造函数!!!!");
						//stage.logWindow.appendText("构造函数!!!!");
                        mysocket=new Socket();   
                        mysocket.addEventListener(Event.CONNECT,OnConnect);    //--当连接成功时 
                        mysocket.addEventListener(IOErrorEvent.IO_ERROR, ioErrorHandler); //连接失败时   
                        mysocket.addEventListener(ProgressEvent.SOCKET_DATA , receivedata);   //返回服务器传输回来的数据 
						mysocket.addEventListener(Event.CLOSE, closeHandler);
                        //mysocket.addEventListener(SecurityError   
                        mysocket.connect(host,port);
                        //发送数据
                        //mysocket.writeUTFBytes("i am flash\n");   
                        //mysocket.flush();//发送数据      
						bytes.position = 0;
                }   
                
				public function SetStage(_stage:Stage)
				{
					mStage = _stage;
					m = new Main(mStage);
				}
				
                private function OnConnect(e:Event):void  
                {   
                        trace("连接成功");   
						//logWindow.appendText("连接成功");
                       // mysocket.writeUTFBytes("Test successful2\n");   
                       // mysocket.flush();//发送数据   
                        
                }   
                
                private function ioErrorHandler(e:IOErrorEvent):void  
                {   
                        trace("连接失败");   
						//logWindow.appendText("连接失败");
                }   
                
                private function receivedata(e:ProgressEvent):void  
                {   
                        trace("收到的字节数"+mysocket.bytesAvailable);   
						
						if( mysocket.bytesAvailable != 0 && mysocket.bytesAvailable % 307200 == 0 )
						{
							mysocket.readBytes(bytes, 0, mysocket.bytesAvailable);
							m.ReceiveBitmap(bytes);
							bytes.position = 0;
						}
                }   
                
                private function SendData(e:MouseEvent):void  
                {   
                        trace("发送");  
						//logWindow.appendText("发送");
                        mysocket.writeUTFBytes("i am flash\n");   
                        mysocket.flush();//发送数据   
                        
                }   
				private function closeHandler(e:Event):void
				{
					trace("Data Receive Finished, bytes:"+mysocket.bytesAvailable);
					trace("Begain Read");
					mysocket.readBytes(bytes, 0, mysocket.bytesAvailable);
					trace("Bytes="+bytes[0]+" "+bytes[1]+" "+bytes[2]+" "+bytes[3]+" "+bytes[4]+" "+bytes[5]);
					trace("Bytes="+bytes[307200-5]+" "+bytes[307200-4]+" "+bytes[307200-3]+" "+bytes[307200-2]+" "+bytes[307200-1]+" "+bytes[307200-0]);
					
                    trace("Receive Finished, total:"+bytes.bytesAvailable);   

					m.ReceiveBitmap(bytes);
				
				}
        }   
}