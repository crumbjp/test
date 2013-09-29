package test;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

import org.apache.mina.core.future.ConnectFuture;
import org.apache.mina.core.service.IoAcceptor;
import org.apache.mina.core.service.IoHandlerAdapter;
import org.apache.mina.core.session.IdleStatus;
import org.apache.mina.core.session.IoSession;
import org.apache.mina.transport.vmpipe.VmPipeAcceptor;
import org.apache.mina.transport.vmpipe.VmPipeAddress;
import org.apache.mina.transport.vmpipe.VmPipeConnector;

public class TestMain {
	static class MsgBase {
		public static enum TYPE {
			UNDEF,
			GET,
			SET,
			FINNISH,
		}
		public TYPE type = MsgBase.TYPE.UNDEF;
		public IoSession session;
		public MsgBase(TYPE t) {
			this.type = t;
		}
	}
	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception {
		IoAcceptor acceptor = new VmPipeAcceptor();
		VmPipeAddress address = new VmPipeAddress(8080);
		// Worker thread
		final BlockingQueue<MsgBase> workerQueue = new ArrayBlockingQueue<MsgBase>(10);
		final Thread th = new Thread(){
			public final BlockingQueue<MsgBase> q = workerQueue; 
			public void run() {
				while (true) {
					try {
						MsgBase m = q.take();
						switch (m.type) {
						case GET:
							Thread.sleep(1000);
							m.session.write("GET finnished.");
							break;
						case SET:
							Thread.sleep(1000);
							m.session.write("SET finnished.");
							break;
						case FINNISH:
							return;
						default:
							break;
						}
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
			}
		};
		th.start();
		// Server
		acceptor.setHandler(new IoHandlerAdapter(){
			private final int id = 0;
			public void sessionOpened(IoSession session) {
				System.out.println("Server-" + id + ": READY");
			}
			public void sessionClosed(IoSession session) {
				System.out.println("Server-" + id + ": QUIT");
		    }
			public void messageReceived(IoSession session, Object message) {
				System.out.println("Server-" + id + ": RCVD " + message);
				try { 
					MsgBase m = (MsgBase)message;
					switch (m.type) {
					case GET:
						m.session = session;
						workerQueue.put(m);
						break;
					case SET:
						m.session = session;
						workerQueue.put(m);
						break;
					default:
						break;
					}
				}catch ( Throwable e){
					;
				}
			}
			public void messageSent(IoSession session, Object message) {
				System.out.println("Server-" + id + ": SENT " + message);
		    }
			public void exceptionCaught(IoSession session, Throwable cause) {
				cause.printStackTrace();
				session.close(true);
		    }		
		});
		acceptor.bind(address);
        
		// Client
		VmPipeConnector connector = new VmPipeConnector();
		connector.setHandler(new IoHandlerAdapter() {
			public void sessionOpened(IoSession arg0) throws Exception {
			}
			public void sessionIdle(IoSession arg0, IdleStatus arg1) throws Exception {
			}
			public void sessionCreated(IoSession arg0) throws Exception {
			}
			public void sessionClosed(IoSession arg0) throws Exception {
			}
			public void messageSent(IoSession arg0, Object arg1) throws Exception {
			}
			public void messageReceived(IoSession arg0, Object message) throws Exception {
				System.out.println("Client-" + 0 + ": RCV " + message);
			}
			@Override
			public void exceptionCaught(IoSession session, Throwable cause) {
				cause.printStackTrace();
				session.close(true);
			}
		});
		ConnectFuture future = connector.connect(address);
		future.awaitUninterruptibly();

		IoSession session = future.getSession();
		session.write(new MsgBase(MsgBase.TYPE.GET));
		session.write(new MsgBase(MsgBase.TYPE.SET));
       session.getCloseFuture().awaitUninterruptibly();

       workerQueue.put(new MsgBase(MsgBase.TYPE.FINNISH));
       acceptor.unbind();
	}
}
