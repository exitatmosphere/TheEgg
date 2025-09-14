class Deferred {
  promise;
  resolve = () => {};
  reject = () => {};
  constructor() {
    this.promise = new Promise((resolve, reject) => {
      this.resolve = resolve;
      this.reject = reject;
    });
  }
}

let workersState = [];

function runTaskWithWorkers(args, numThreads) {
  const threads = numThreads || window.navigator.hardwareConcurrency;
  const triesBatch = 10000;
  const dfd = new Deferred();

  for (let i = 0; i < threads; i++) {
    const newWorker = new Worker("worker.js");
    workersState.push({ worker: newWorker, timeSpent: 0, tries: 0 });
    const timeStart = performance.now();

    newWorker.onmessage = (msg) => {
      const workerState = workersState[i];
      workerState.timeSpent = performance.now() - timeStart;

      const msgData = msg.data;
      if (msgData.error) {
        dfd.resolve(msgData);
        return;
      }

      const msgResult = msgData.result;

      if (msgResult.seed === "") {
        workerState.tries += triesBatch;
        workerStartTask(newWorker, threads, triesBatch, args);
      } else {
        const status = taskStatus();
        msgResult.tries += status.tries;
        msgResult.time = status.timeSpent;

        stopWorkers();

        dfd.resolve(msgResult);
      }
    };

    workerStartTask(newWorker, threads, triesBatch, args);
  }

  return dfd.promise;
}

function stopWorkers() {
  for (const workerState of workersState) {
    workerState.worker.terminate();
  }
  workersState = [];
  console.log("Terminated all workers");
}

function taskStatus() {
  let tries = 0;
  let timeSpent = 0;

  workersState.forEach((workerState) => {
    tries += workerState.tries;
    const timeSpentLocal = workerState.timeSpent;
    if (timeSpentLocal > timeSpent) {
      timeSpent = timeSpentLocal;
    }
  });

  return {
    tries,
    timeSpent: parseDomTimestamp(timeSpent),
  };
}

function viewWorkersState() {
  for (let i = 0; i < workersState.length; i++) {
    const workerState = workersState[i];
    console.log(
      `${i}) tries: ${workerState.tries}, time: ${parseDomTimestamp(
        workerState.timeSpent
      )}`
    );
  }
}

function workerStartTask(worker, threads, triesBatch, args) {
  const srandSeed = Math.floor(Math.random() * (2 ** 32 / 2) - threads + 1);
  worker.postMessage({ args: [srandSeed, triesBatch, ...args] });
}

function parseDomTimestamp(timestamp) {
  return (timestamp / 1000).toFixed(3);
}
