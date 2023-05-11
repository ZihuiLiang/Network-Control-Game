#![allow(unused_variables)]
#![allow(unused_must_use)]
use std::{cmp};
use std::collections::{HashMap};
use std::thread;
use std::sync::{RwLock, Arc, Mutex, Condvar};

const SEARCH_SIZE: u8 = 100;
static T: u8 = 1;
const THREAD_NUM: u8 = 64;
const THREAD_MASK: u8 = THREAD_NUM - 1;


fn main() {
    
    let mut solution = NetworkControl::new();
    solution.start();
    println!("t:{} search size:{}", T, SEARCH_SIZE);
    for sz in 1..=SEARCH_SIZE {
        build_config_queues(sz, &mut solution.config_queues);
        notify_all(&solution.distri_sems);
        wait_all(&solution.gather_sems);
        notify_all(&solution.distri_sems);
        wait_all(&solution.gather_sems);
    }
    println!("zero graphs:");
    let zero_graph = solution.zero_graph.read().unwrap().clone();
    for (i, graph) in zero_graph.iter().enumerate() {
        println!("{}th zero_graph: {}", i, format_config(graph));
    }
}

struct NetworkControl {
    f: Arc<Vec<Arc<RwLock<HashMap<String, u8> > > > >,
    zero_graph: Arc<RwLock<Vec<Vec<u8> > > >, 
    config_queues: Vec<Arc<RwLock<Vec<Vec<u8> > > > >,
    distri_sems: Vec<Arc<(Mutex<bool>, Condvar)> >,
    gather_sems: Vec<Arc<(Mutex<bool>, Condvar)> >
}

impl NetworkControl {
    fn new() -> Self{
        let mut _f = vec![];
        for i in 0..THREAD_NUM {
            let hm: HashMap<String, u8> = HashMap::new(); 
            _f.push(Arc::new(RwLock::new(hm.clone())));
        }
        let mut t = NetworkControl{
            f: Arc::new(_f.clone()), 
            zero_graph: Arc::new(RwLock::new(vec![vec![]])),
            config_queues: vec![],
            distri_sems: vec![],
            gather_sems: vec![]
        };
        for i in 0..THREAD_NUM {
            t.distri_sems.push(Arc::new((Mutex::new(false), Condvar::new())));
            t.gather_sems.push(Arc::new((Mutex::new(false), Condvar::new())));
            t.config_queues.push(Arc::new(RwLock::new(vec![vec![]])));
        }
        t
    }  
    
    fn start(&mut self) {
        for thread_id in 0..THREAD_NUM {
            let thread_name = format!("{}", thread_id);
            let distri_sem = Arc::clone(&self.distri_sems[thread_id as usize]);
            let gather_sem = Arc::clone(&self.gather_sems[thread_id as usize]);
            let config_queue = Arc::clone(&self.config_queues[thread_id as usize]);
            let zero_graph = Arc::clone(&self.zero_graph);
            let f = Arc::clone(&self.f);
            thread::Builder::new().name(thread_name).spawn(move|| {
                let thread_id = thread::current().name().unwrap().parse::<u8>().unwrap();
                loop {
                    let mut config_value: Vec< (Vec<u8>, u8)> = vec![];
                    wait(&distri_sem);
                    search_value(&config_queue, &mut config_value, &f, thread_id);
                    notify(&gather_sem);
                    wait(&distri_sem);
                    put_value(&config_value, &f, &zero_graph, thread_id);
                    notify(&gather_sem);
                }
            });
        }
    }

}

fn config2threadid(d:& Vec<u8>) -> u8 {
    let mut id = 0u8;
    for i in d {
        id=(id<<3)^(id>>4)^*i; 
    }
    id & THREAD_MASK
}

fn clear(d:&mut Vec<u8>) {
    if d.is_empty() {
        return;
    }
    d.sort();
    let mut l:i32 = 0;
    for i in 1..d.len() {
        if l == -1 || d[i] != d[l as usize] {
            l += 1;
            d[l as usize] = d[i];
        } else {
            l -= 1;
        }
    }
    d.resize((l + 1) as usize, 0u8);
}

fn notify(sem:& (Mutex<bool>, Condvar)) {
    let (lock, cvar) = &*sem;    
    *lock.lock().unwrap() = true;
    cvar.notify_one();
}

fn notify_all(sems:& Vec<Arc<(Mutex<bool>, Condvar)> >) {
    for sem in sems {
        notify(sem);
    }
}

fn wait(sem:& (Mutex<bool>, Condvar)) {
    let (lock, cvar) = &*sem;
    let mut started = *lock.lock().unwrap();
    while !started {
        started = *cvar.wait(lock.lock().unwrap()).unwrap();
    }
    *lock.lock().unwrap() = false;
}

fn wait_all(sems:& Vec<Arc<(Mutex<bool>, Condvar)> >) {
    for sem in sems {
        wait(sem);
    }
}

fn split(n: u8,mn: u8,d: &mut Vec<u8>, config_queues:&mut Vec<Vec<Vec<u8> > >) {
    if n==0 {
        config_queues[config2threadid(d) as usize].push(d.clone());
        return;
    }
    if mn>n {
        return;
    }
    for i in mn..=n/2 {
        d.push(i);
        split(n-i, i+1, d, config_queues);
        d.pop();
    }
    d.push(n);
    split(0, n+1, d, config_queues);
    d.pop();
}

fn build_config_queues(sz: u8, config_queues:&mut Vec<Arc<RwLock<Vec<Vec<u8> > > > >) {
    let mut d: Vec<u8> = vec![];
    let mut config_queues_:Vec<Vec<Vec<u8>>> = vec![vec![]; THREAD_NUM as usize];
    split(sz, 1, &mut d, &mut config_queues_);
    for i in 0..THREAD_NUM {
        *config_queues[i as usize].write().unwrap() = config_queues_[i as usize].clone();
    }
}

fn search_value(config_queue:& Arc< RwLock< Vec<Vec<u8> > > >,config_value:&mut Vec< (Vec<u8>, u8)>, f:&Vec<Arc<RwLock<HashMap<String, u8> > > >, thread_id: u8) {
    let config_queue = config_queue.read().unwrap().clone();
    for config in &config_queue {
        let mut d = vec![];
        let mut v = 0;
        'outfor:for (i, di) in config.iter().enumerate() {
            for j in 1..=(*di+1)/2 {
                let mut u = d.clone();
                let mut vv = 1;
                if j > T+1 {
                    u.push(j-T-1);
                    vv += T;
                } else {
                    vv += j-1;
                }
                if j + T < *di {
                    u.push(*di-j-T);
                    vv += T;
                } else {
                    vv += *di-j;
                }
                if vv <= v {
                    continue;
                }
                for k in i+1..config.len() {
                    u.push(config[k]);
                }
                clear(&mut u);
                if u.is_empty() {
                    v = vv;
                } else {
                    let key: String = (String::from_utf8_lossy(&u).to_string()).clone();
                    let oponent_v = *((f[config2threadid(&u) as usize]).read().unwrap().get(&key)).unwrap();
                    if oponent_v < vv {
                        v = cmp::max(v, vv-oponent_v);
                    }
                }
                if v == T*2+1 {
                    break 'outfor;
                }
            }
            d.push(*di);
        }
        config_value.push((config.clone(), v));
    } 
}

fn put_value(config_value:& Vec< (Vec<u8>, u8)>, f:&Vec<Arc<RwLock<HashMap<String, u8> > > >, zero_graph:& Arc<RwLock<Vec<Vec<u8> > > >, thread_id: u8) {
    let mut _f = f[thread_id as usize].write().unwrap();
    for (config, value) in config_value {
        _f.insert((String::from_utf8_lossy(&config).to_string()).clone(), *value);
        if *value == 0 {
            zero_graph.write().unwrap().push(config.clone());
        }
    }
}

fn format_config(config:& Vec<u8>) -> String {
    let mut str: String = "".to_string();
    for i in config {
        str = format!("{} {}", str, (*i as u32).to_string());
    }
    str
}