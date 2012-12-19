
var utils = require("utils.js");

var validationError = {
    'OK' : 0,
    'TOO_MANY_ARGUMENTS' : 1,
    'DIFFERENT_TYPE' : 2,
    'INVALID_VALUE' : 3,
    'ALL_TYPES_INCOMPATIBLE' : 4,
    'NOT_AN_OBJECT' : 5,
    'MISSING_PROPERTY' : 6,
    'NOT_NULL' : 7
  };

//Used when value can be of more than one type -
//does checking for all types and succeeds when at least one succeeds.
var MultiTypeValidator = function(aSpec) {
  var spec = aSpec;
  var types = aSpec.type;
  var validators = [];
  for(var i = 0; i < types.length; ++i) {
    validators.push(validatorManager.getValidator(types[i]));
  }

  this.validate = function(aInstance) {
    var validationReport = { 'success': true, 'error' : null, 'errorCode' : validationError.OK };
    var reports = [];
    for(var i = 0; i < validators.length; ++i) {
      var report = validators[i].validate(aInstance);
      if (report.success) {
        return report;
      }
      reports.push(report);
    }
    if (reports.length > 0) {
      validationReport.success = false;
      validationReport.errorCode = validationError.ALL_TYPES_INCOMPATIBLE;
      validationReport.error = [];
      for(var i = 0; i < reports.length; ++i) {
        validationReport.error.push(reports.error);
      }
    }
    return validationReport;
  }
}

function createValidationReportSuccess(){
  return { 'success': true, 'error' : null, 'errorCode' : validationError.OK };
}

function createValidationReportError(aError, aErrorCode){
  return { 'success': false, 'error' : aError, 'errorCode' : aErrorCode };
}

function simpleTypeValidation(aTypename, aArg) {
  if (utils.typeName(aArg) === aTypename) {
    return createValidationReportSuccess();
  } else {
    var e = "Type \'" + utils.typeName(aArg) + "\' differs from \'" + aTypename + "\'";
    return createValidationReportError(e, validationError.DIFFERENT_TYPE);
  }
}
//--------------------------------------------
// Manages all validators
var ValidatorManager = function() {

  var validators = {};

  this.getValidator = function(aSpec) {
    if (!aSpec) {
      throw new Error('Unspecified validator name!');
    }
    if (utils.isString(aSpec)) {
      console.log("getting validator for " + aSpec);
      if (!validators[aSpec]) {
        throw new Error('Unavailable validator :' + aSpec);
      }
      return new validators[aSpec]();
    }
    if (!utils.isString(aSpec.type)) {
      if (utils.isArray(aSpec.type)) {
        return new MultiTypeValidator(aSpec);
      }
      if (aSpec.type.type) {
        return new validators[aSpec.type.type](aSpec.type);
      }
      throw new Error('Unsupported type specifier');
    }
    if (!validators[aSpec.type]) {
      throw new Error('Unsupported type specification: ' + aSpec.type);
    }
    return new validators[aSpec.type](aSpec);
  }

  this.addValidator = function(aValidatorName, aValidator) {
    if (!aValidator) {
      throw new Error('Wrong validator!');
    }
    if (!aValidatorName || !utils.isString(aValidatorName)) {
      throw new Error('Wrong validator name!');
    }
    console.debug('Adding validator :' + aValidatorName);
    validators[aValidatorName] = aValidator;
  }


  this.addSpecValidatorWrapper = function(aName, aSpecification) {
    if (!aName || !utils.isString(aName)) {
      throw new Error('Wrong validator name!');
    }
    var validatorBase;
    var specification;
    if (utils.isString(aSpecification.type)) {
      validatorBase = validators[aSpecification.type];
      specification = aSpecification;
    } else {
      validatorBase = validators[aSpecification.type.type];
      specification = aSpecification.type;
    }
    if (!validatorBase) {
      throw new Error('Validator wrapper \'' + aName + '\' needs existing validator : \'' + aSpecification.type + '\'')
    }
    var validatorConstructor = function() {
      validatorBase.call(this, specification);
    }
    this.addValidator(aName, validatorConstructor);
  }



  //---------------------------------------
  //  Initialize manager by basic validators
  this.addValidator('any', function() {
    this.validate = function(aArg) {
      if (aArg === null || aArg === undefined) {
        var e = "Specified object is not \'null\'";
        return createValidationReportError(e, validationError.INVALID_VALUE);
      } else {
        return createValidationReportSuccess();
      }
    }
  });

  this.addValidator('number', function() {
    this.validate = function(aArg) {
      return simpleTypeValidation('number', aArg);
    }
  });

  this.addValidator('string', function() {
    this.validate = function(aArg) {
      return simpleTypeValidation('string', aArg);
    }
  });

  this.addValidator('function', function() {
    this.validate = function(aArg) {
      return simpleTypeValidation('function', aArg);
    }
  });

  this.addValidator('boolean', function() {
    this.validate = function(aArg) {
      return simpleTypeValidation('boolean', aArg);
    }
  });

  this.addValidator('null', function() {
    this.validate = function(aArg) {
      if (utils.isNull(aArg)) {
        return createValidationReportSuccess();
      } else {
        var e = "Specified object is not \'null\'";
        return createValidationReportError(e, validationError.NOT_NULL);
      }
    }
  });

  this.addValidator('integer', function() {
    this.validate = function(aArg) {
      if (utils.isInteger(aArg)) {
        return createValidationReportSuccess();
      } else {
        var e = "Specified object \'" + aArg + "\'is not an \'integer\'";
        return createValidationReportError(e, validationError.DIFFERENT_TYPE);
      }
    }
  });

  this.addValidator('enumerated string', function(aSpec) {
    var specification = aSpec;

    this.validate = function(aArg) {
      var report = simpleTypeValidation('string', aArg);
      if (!report.success) {
        return report;
      }

      var found = false;
      for (var i = 0; !found && (i < specification['enum'].length); ++i) {
        found = found || specification['enum'][i] === aArg;
      }
      if (found) {
        return report;
      }
      var e = "Value \'" + aArg + "\' is not allowed for enumerated string";
      return createValidationReportError(e, validationError.INVALID_VALUE);
    }
  });
}
var validatorManager = new ValidatorManager;
exports.validatorManager = validatorManager;


var FunctionArgumentsValidator = function(aSpecification) {

  var specification = aSpecification;

  this.getFunctionCallString = function(aFunctionName, aArguments) {
    var tmp = aFunctionName + '(';
    for (var i = 0; i < aArguments.length - 1; ++i) {
      tmp += utils.typeName(aArguments[i]) + ', ';
    }
    tmp += aArguments.length > 0 ? (utils.typeName(aArguments[i]) + ')') : ')';
    return tmp;
  }

  this.getTypeDescription = function(aType) {
    if (utils.isString(aType)) {
      return aType;
    }
    if (utils.isArray(aType)) {
      if (aType.length == 0) {
        throw Error('Empty type array!');
      }
      var result = this.getTypeDescription(aType[0]);
      for (var i = 1; i < aType.length; ++i) {
        result += ' or ' + this.getTypeDescription(aType[i]);
      }
      return result;
    }
    if (aType.type) {
      if (aType.type === 'array' && utils.isString(aType.items)) {
        return 'array of ' + aType.items;
      }
      return this.getTypeDescription(aType.type);
    }
    throw new Error('Unsupported type decription!');
  }

  this.getFunctionSpecificationString = function() {
    var tmp = specification.id + '(';
    for (var i = 0; i < specification.items.length; ++i) {
      var spec = specification.items[i];
      var modifier = spec.required ? '' : 'optional ';
      var delimiter = ((i + 1 != specification.items.length) ? ', ' : '')
      tmp += modifier + this.getTypeDescription(spec.type) + ' ' + spec.id + delimiter;
    }
    tmp += ')';
    return tmp;
  }

  this.getIncompatibleCallErrorMessage = function(aArguments) {
    var tmp = 'Invocation of form '
      + this.getFunctionCallString(specification.id, aArguments)
      + ' doesn\'t match definition '
      + this.getFunctionSpecificationString(specification);
    return tmp;
  }



  this.validate = function(aArguments) {
    var validationReport = { 'success': true, 'processedArguments': {}, 'error': null, 'errorCode': validationError.OK };
    var processedArguments = {};
    var argIdx = 0;
    var valResult = null;
    for (var i = 0; i < specification.items.length; ++i) {

      var spec = specification.items[i];
      var validator = validatorManager.getValidator(spec);

      valResult = validator.validate(aArguments[argIdx]);

      if (valResult.success) {
        processedArguments[spec.id] = aArguments[argIdx];
        ++argIdx;
      } else {
        if (!spec.required) {
          //If argument was optional change to success
          valResult.success = true;

          //We were testing an optional argument
          processedArguments[spec.id] = spec['default'];
          //if optional argument passed as undefined we need to move to the next argument
          if (aArguments[argIdx] === undefined) {
            ++argIdx;
          }
        } else {
          //validation failed - we stop  processing and create error report
          break;
        }
      }
    }
    //Inspect last validation error
    if (!valResult.success) {
      console.error("ERROR :" + valResult.error);
      validationReport.errorCode = valResult.errorCode;
      validationReport.success = false;


      switch (valResult.errorCode) {
        case validationError.INVALID_VALUE:
          validationReport.error = ('Invalid value for argument ' + (argIdx + 1) + '. Value does not match any valid type choices.');
          break;
        default:
          validationReport.error = this.getIncompatibleCallErrorMessage(aArguments);
          break;
      }
      return validationReport;
    }
    //Not all of the arguments were processed
    if (argIdx < aArguments.length) {
      validationReport.success = false;
      validationReport.errorCode = validationError.TOO_MANY_ARGUMENTS
      validationReport.error = this.getIncompatibleCallErrorMessage(aArguments);
      return validationReport;
    }

    validationReport.processedArguments = processedArguments;
    return validationReport;
  }
}
validatorManager.addValidator('functionArguments', FunctionArgumentsValidator);

var ObjectValidator = function(aSpec) {
  var specification = aSpec;
  this.validate = function (aObject) {
    var validationReport = createValidationReportSuccess();

    if (!utils.isObject(aObject)) {
      var e = 'Object expected instead of :' + utils.typeName(aObject);
      return createValidationReportError(e, validationError.NOT_AN_OBJECT);
    }

    properties = specification.properties;
    if (!properties) {
      return validationReport;
    }

    for (var i in properties) {
      property = aObject[i];
      propertySpecification = properties[i];
      if (property !== undefined) {
        validator = validatorManager.getValidator(propertySpecification);
        var report = validator.validate(property);
        if (!report.success) {
          var e = 'Wrong type for property \'' + i + '\'!';
          return createValidationReportError(e, validationError.DIFFERENT_TYPE);
        }
      } else {
      if (propertySpecification.required) {
          var e = 'Missing property \'' + i + '\'!';
          return createValidationReportError(e, validationError.MISSING_PROPERTY);
        }
      }
    }
    return validationReport;
  }
}
validatorManager.addValidator('object', ObjectValidator);

var ArrayValidator = function(aSpec) {
  var specification = aSpec;

  this.validate = function(aArray) {
    var validationReport = createValidationReportSuccess();
    if (!utils.isArray(aArray)) {
      return createValidationReportError('Not an array!', validationError.DIFFERENT_TYPE);
    }
    if (!specification.items) {
      return validationReport;
    }
    if (!utils.isArray(specification.items)) {
      var spec = specification.items;
      var validator = validatorManager.getValidator(spec);
      for (var i = 0; i < aArray.length; ++i) {
        var report = validator.validate(aArray[i]);
        if (!report.success) {
          return report;
        }
      }
      return validationReport;
    }

    for (var i = 0; i < specification.items.length; ++i) {
      var spec = specification.items[i];
      var validator = validatorManager.getValidator(spec);

      valResult = validator.validate(aArray[argIdx]);

      if (valResult.success) {
        ++argIdx;
      } else {
        if (!spec.required) {
          //if optional argument passed as undefined we need to move to the next argument
          if(aArguments[argIdx] === undefined) {
            ++argIdx;
          }
        } else {
          return valResult;
        }
      }
    }
    return validationReport;
  }
}
validatorManager.addValidator('array', ArrayValidator);


var ImageDataValidator = function() {
  this.validate = function(aArg) {
    //TODO - implement properly
    return createValidationReportSuccess();
  }
};
validatorManager.addValidator('imagedata', ImageDataValidator);

//Validation procedure - throws an exception when validation error uccured.
var preprocessArguments = function(aMethodName, aArguments) {
  var validator = null;
  try {
    validator = validatorManager.getValidator(aMethodName);
  } catch (e) {
    console.error('Cannot obtain right validator for :' + aMethodName + '\n\t' + e.message);
    throw e;
  }
  var report = validator.validate(aArguments);

  if (report.success) {
    return report.processedArguments;
  }
  console.error(report.error);
  throw new Error(report.error);
}
exports.preprocessArguments = preprocessArguments;

//Used in methods which are not implemented yet - it throws an exception,
//but also checks passed arguments - so caller atleast knows that he didn't make a mistake.
exports.notImplemented = function(aMethodName, aArguments) {
  var message = 'Function \'' + aMethodName + '\' not yet implemented. Arguments were OK.';
  try {
    preprocessArguments(aMethodName, aArguments);
  } catch (e) {
    message = 'Function \'' + aMethodName + '\' not yet implemented. Also problem with arguments : ' + (e.message || e.description);
  }
  console.error(message);
  throw new Error(message);
}
