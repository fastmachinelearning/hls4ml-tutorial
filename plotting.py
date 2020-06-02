import numpy as np
import matplotlib.pyplot as plt
from sklearn.metrics import roc_curve, auc
import pandas as pd
from sklearn.metrics import confusion_matrix
import itertools

# confusion matrix code from Maurizio
# /eos/user/m/mpierini/DeepLearning/ML4FPGA/jupyter/HbbTagger_Conv1D.ipynb
def plot_confusion_matrix(cm, classes,
                          normalize=False, 
                          title='Confusion matrix',
                          cmap=plt.cm.Blues):
    """
    This function prints and plots the confusion matrix.
    Normalization can be applied by setting `normalize=True`.
    """
    if normalize:
        cm = cm.astype('float') / cm.sum(axis=1)[:, np.newaxis]

    plt.imshow(cm, interpolation='nearest', cmap=cmap)
    #plt.title(title)
    cbar = plt.colorbar()
    plt.clim(0,1)
    cbar.set_label(title)
    tick_marks = np.arange(len(classes))
    plt.xticks(tick_marks, classes, rotation=45)
    plt.yticks(tick_marks, classes)

    fmt = '.2f' if normalize else 'd'
    thresh = cm.max() / 2.
    for i, j in itertools.product(range(cm.shape[0]), range(cm.shape[1])):
        plt.text(j, i, format(cm[i, j], fmt),
                 horizontalalignment="center",
                 color="white" if cm[i, j] > thresh else "black")

    #plt.tight_layout()
    plt.ylabel('True label')
    plt.xlabel('Predicted label')

def plotRoc(fpr, tpr, auc, labels, linestyle):
    for i, label in enumerate(labels):
        plt.plot(tpr[label],fpr[label],label='%s tagger, AUC = %.1f%%'%(label.replace('j_',''),auc[label]*100.),linestyle=linestyle)
    plt.semilogy()
    plt.xlabel("Signal Efficiency")
    plt.ylabel("Background Efficiency")
    plt.ylim(0.001,1)
    plt.grid(True)
    plt.legend(loc='upper left')
    plt.figtext(0.25, 0.90,'hls4ml',fontweight='bold', wrap=True, horizontalalignment='right', fontsize=14)

def rocData(X, y, labels, model):
    predict_test = model.predict(X)

    df = pd.DataFrame()

    fpr = {}
    tpr = {}
    auc1 = {}

    for i, label in enumerate(labels):
        df[label] = y[:,i]
        df[label + '_pred'] = predict_test[:,i]

        fpr[label], tpr[label], threshold = roc_curve(df[label],df[label+'_pred'])

        auc1[label] = auc(fpr[label], tpr[label])
    return fpr, tpr, auc1, predict_test

def makeRoc(X, y, labels, model, linestyle='-'):

    if 'j_index' in labels: labels.remove('j_index')
        
    fpr, tpr, auc1, predict_test = rocData(X, y, labels, model)
    plotRoc(fpr, tpr, auc1, labels, linestyle)
    return predict_test
    
def _byteify(data, ignore_dicts = False):
    # if this is a unicode string, return its string representation
    if isinstance(data, six.text_type):
        return data.encode('utf-8')
    # if this is a list of values, return list of byteified values
    if isinstance(data, list):
        return [ _byteify(item, ignore_dicts=True) for item in data ]
    # if this is a dictionary, return dictionary of byteified keys and values
    # but only if we haven't already byteified it
    if isinstance(data, dict) and not ignore_dicts:
        return {
            _byteify(key, ignore_dicts=True): _byteify(value, ignore_dicts=True)
            for key, value in data.items()
        }
    # if it's anything else, return it in its original form
    return data


def eval(model, options, yamlConfig):
    X_train_val, X_test, y_train_val, y_test, labels  = get_features(options, yamlConfig)
    y_predict = makeRoc(X_test, labels, y_test, model, options.outputDir)
    y_test_proba = y_test.argmax(axis=1)
    y_predict_proba = y_predict.argmax(axis=1)
    # Compute non-normalized confusion matrix
    cnf_matrix = confusion_matrix(y_test_proba, y_predict_proba)
    np.set_printoptions(precision=2)
    # Plot non-normalized confusion matrix
    plt.figure()
    plot_confusion_matrix(cnf_matrix, classes=[l.replace('j_','') for l in labels],
                              title='Confusion matrix')
    plt.figtext(0.28, 0.90,'hls4ml',fontweight='bold', wrap=True, horizontalalignment='right', fontsize=14)
    #plt.figtext(0.38, 0.90,'preliminary', style='italic', wrap=True, horizontalalignment='center', fontsize=14) 
    plt.savefig(options.outputDir+"/confusion_matrix.pdf")
    # Plot normalized confusion matrix
    plt.figure()
    plot_confusion_matrix(cnf_matrix, classes=[l.replace('j_','') for l in labels], normalize=True,
                              title='Normalized confusion matrix')

    plt.figtext(0.28, 0.90,'hls4ml',fontweight='bold', wrap=True, horizontalalignment='right', fontsize=14)
    #plt.figtext(0.38, 0.90,'preliminary', style='italic', wrap=True, horizontalalignment='center', fontsize=14) 
    plt.savefig(options.outputDir+"/confusion_matrix_norm.pdf")
        
def eval_extra(model, options, yamlConfig):
    import json

    if os.path.isfile('%s/full_info.log'%os.path.dirname(options.inputModel)):
        f = open('%s/full_info.log'%os.path.dirname(options.inputModel))
        myListOfDicts = json.load(f, object_hook=_byteify)
        myDictOfLists = {}
        for key, val in myListOfDicts[0].items():
            myDictOfLists[key] = []
        for i, myDict in enumerate(myListOfDicts):
            for key, val in myDict.items():
                myDictOfLists[key].append(myDict[key])

        plt.figure()
        val_loss = np.asarray(myDictOfLists[b'val_loss'])
        loss = np.asarray(myDictOfLists[b'loss'])
        plt.plot(val_loss, label='validation')
        plt.plot(loss, label='train')
        plt.legend()
        plt.xlabel('epoch')
        plt.ylabel('loss')
        plt.savefig(options.outputDir+"/loss.pdf")

        plt.figure()
        val_acc = np.asarray(myDictOfLists[b'val_accuracy'])
        acc = np.asarray(myDictOfLists[b'accuracy'])
        plt.plot(val_acc, label='validation')
        plt.plot(acc, label='train')
        plt.legend()
        plt.xlabel('epoch')
        plt.ylabel('accuracy')
        plt.savefig(options.outputDir+"/acc.pdf")

