package {
	import flash.events.MouseEvent;
	import flash.display.*;
	import flash.utils.*;
	import flash.display.Stage;
	import flash.display.StageAlign;
	import flash.display.StageDisplayState;
	import flash.geom.Rectangle;
	//import flash.filesystem.*;

	public class Main extends Sprite {
		public var contentBox:Sprite=new Sprite;

		private var _bmpd:BitmapData;
		private var mBMP:Bitmap;
		private var mStage:Stage;

		public function Main(_stage:Stage) {
			mStage = _stage;
			addChild(contentBox);

			_bmpd=new BitmapData(320,240,true,0x000000);
			mBMP = new Bitmap(_bmpd);
			mStage.addChild(mBMP);
			//for (var i:int=0; i<3; i++) {
			//DrawDetector(100*i,100*i);
			//}

			//stage.addEventListener(MouseEvent.CLICK,Clean);
			//CleanDetector();

		}
		public function AttachBMP()
		{
			trace("AttachBMP");
			var myBitmapData:BitmapData = new BitmapData(40, 40, false, 0x0000FF00);

			var rect:Rectangle = new Rectangle(0, 0, 20, 20);
			myBitmapData.fillRect(rect, 0x0000FF);

			var bm:Bitmap = new Bitmap(myBitmapData);
			mStage.addChild(bm);
		}

		public function Clean(event) {
			CleanDetector();
			trace("dianle");
		}

		//画标记
		public function DrawDetector(_x:int,_y:int,_width:Number=10,_height:Number=10) {
			var square:Sprite= new Sprite();
			square.graphics.beginFill(0xFF88CC);
			square.graphics.drawRect(0, 0, _width, _height);

			square.x=_x;
			square.y=_y;
			contentBox.addChild(square);
		}

		//清屏
		public function CleanDetector() {
			while (contentBox.numChildren>0) {
				contentBox.removeChildAt(0);
			}

		}
		public function ReceiveBitmap(bit:*) {
			trace("Main::ReceiveBitmap:Receive "+bit.bytesAvailable+" bytes");
			
			var rect:Rectangle = new Rectangle(0, 0, 320, 240);
			
             bit.position = 0;
			_bmpd.setPixels(rect,bit);
			

			// var myFile:File = File.documentsDirectory.resolvePath("flashoutput.bmp");
			// var myFileStream:FileStream = new FileStream();
			//myFileStream.open(myFile, FileMode.WRITE);
			//myFileStream.writeBytes(bit, 0, bit.length);
		}

	}
}