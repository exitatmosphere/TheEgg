let immolateInitialized = false;

self.Immolate = {
  onRuntimeInitialized: function () {
    immolateInitialized = true;
  },
};

importScripts("immolate.js");

async function runTask(args) {
  for (let c = 0; c < 5; c++) {
    if (!immolateInitialized) {
      await new Promise((r) => setTimeout(r, 100));
    } else {
      break;
    }
  }
  if (!immolateInitialized) {
    postMessage({
      error: "Failed to initialize immolate instance",
    });
    return;
  }

  const srandSeed = args[0];
  const triesBatch = args[1];

  const jokersPackToFindOneOf = new self.Immolate.VectorStr();
  args[2].forEach((joker) => jokersPackToFindOneOf.push_back(joker));
  const jokerPackToFindNegative = args[3];
  const jokerPackToFindAnkh = args[4];

  const legendariesToFind = new self.Immolate.VectorStr();
  args[5].forEach((legendary) => legendariesToFind.push_back(legendary));
  const legendariesToFindNegatives = args[6];
  const legendariesToFindMaxAnte = args[7];

  const vouchersToFind = new self.Immolate.VectorStr();
  args[8].forEach((voucher) => vouchersToFind.push_back(voucher));
  const vouchersToFindMaxAnte = args[9];

  const jokersShopToFind = new self.Immolate.VectorStr();
  args[10].forEach((joker) => jokersShopToFind.push_back(joker));
  const jokersShopToFindMaxAnte = args[11];

  const result = self.Immolate.findSeed(
    srandSeed,
    triesBatch,
    jokersPackToFindOneOf,
    jokerPackToFindNegative,
    jokerPackToFindAnkh,
    legendariesToFind,
    legendariesToFindNegatives,
    legendariesToFindMaxAnte,
    vouchersToFind,
    vouchersToFindMaxAnte,
    jokersShopToFind,
    jokersShopToFindMaxAnte
  );

  const tries = result.tries;
  const seed = result.seed;

  const legendariesPacksVec = result.legendariesPacks;
  const legendariesPacksArray = [];
  for (let i = 0; i < legendariesPacksVec.size(); i++) {
    legendariesPacksArray.push(legendariesPacksVec.get(i));
  }

  const voucherAntesVec = result.voucherAntes;
  const voucherAntesArray = [];
  for (let i = 0; i < voucherAntesVec.size(); i++) {
    voucherAntesArray.push(voucherAntesVec.get(i));
  }

  const jokerShopIdsVec = result.jokerShopIds;
  const jokerShopIdsArray = [];
  for (let i = 0; i < jokerShopIdsVec.size(); i++) {
    jokerShopIdsArray.push(jokerShopIdsVec.get(i));
  }

  // clean memory
  jokersPackToFindOneOf.delete();
  legendariesToFind.delete();
  vouchersToFind.delete();
  jokersShopToFind.delete();

  legendariesPacksVec.delete();
  voucherAntesVec.delete();
  jokerShopIdsVec.delete();

  result.delete();

  postMessage({
    result: {
      tries,
      seed,
      legendariesPacks: legendariesPacksArray,
      voucherAntes: voucherAntesArray,
      jokerShopIds: jokerShopIdsArray,
    },
  });
}

onmessage = async ({ data }) => {
  runTask(data.args);
};
